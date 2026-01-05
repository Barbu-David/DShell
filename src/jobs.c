#define _POSIX_C_SOURCE 200809L
#include "jobs.h"
#include "sf_wraps.h"
#include "args.h"
#include "parser.h"
#include "read_write.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>

#include <signal.h>


#define MAX_PIPELINE_LENGTH 50

Job* init_job(int commands_num)
{
  Job* job = sf_malloc(sizeof(Job));
  job->commands = sf_malloc(sizeof(Command*) * commands_num); 
  job->command_num = commands_num;

  for (int k = 0; k < commands_num; k++) job->commands[k] = NULL;

  job->history = true;
  job->background = false;
  job->id = 0;
  job->pgid = 0;
  job->state = RUNNING;
  return job;
}

void copy_job(Job* src_job, Job* dst_job) {
  if (!src_job || !dst_job) return;

  for (int i = 0; i < dst_job->command_num; i++) {
    if (dst_job->commands[i] != NULL) {
      free_command(dst_job->commands[i]);
      dst_job->commands[i] = NULL;
    }
  }

  if (dst_job->command_num != src_job->command_num) {
    free(dst_job->commands);
    dst_job->commands = sf_malloc(sizeof(Command*) * src_job->command_num);
    dst_job->command_num = src_job->command_num;
  }

  for (int i = 0; i < dst_job->command_num; i++) {
    dst_job->commands[i] = NULL;
  }

  for (int i = 0; i < src_job->command_num; i++) {
    if (src_job->commands[i] != NULL) {
      dst_job->commands[i] = init_command();
      copy_command(src_job->commands[i], dst_job->commands[i]);
    }
  }

  dst_job->background = src_job->background;
  dst_job->id = -1;
  dst_job->pgid = 0;
}


void free_job(Job* job)
{
  for(int i = 0; i<job->command_num; i++) free_command(job->commands[i]);
  free(job->commands);
  free(job);
}

Job* build_job(char** raw_args, Shell* dshell)
{

  if (!raw_args) return NULL;
  if (raw_args[0] == NULL){ 
    free(raw_args);
    return NULL;
  }

  int cmd_num = 0;      
  int i = 0;
  int start = 0;

  char*** cmd_args = sf_malloc(sizeof(char**) * MAX_PIPELINE_LENGTH);

  while (raw_args[i]) {

    if (strcmp(raw_args[i], "|") == 0) {

      if (cmd_num + 1 >= MAX_PIPELINE_LENGTH) {
        print_error("Pipeline too long");
        free(cmd_args);
        free(raw_args);
        return NULL;
      }

      cmd_args[cmd_num] = init_args(i - start);

      int k = 0;
      for (int h = start; h < i; h++)
        cmd_args[cmd_num][k++] = raw_args[h];

      cmd_num++;
      start = i + 1;
    }

    i++;
  }

  cmd_args[cmd_num] = init_args(i - start);

  int k = 0;
  for (int h = start; h < i; h++)
    cmd_args[cmd_num][k++] = raw_args[h];

  Job* job = init_job(cmd_num + 1);
  job->background = (strcmp(raw_args[i - 1], "&") == 0);
  
  if (job->command_num > 1) {

    job->commands[0] =
      build_command(cmd_args[0], dshell, first_parser);

    job->commands[job->command_num - 1] =
      build_command(cmd_args[job->command_num - 1], dshell, final_parser);

    for (int j = 1; j < job->command_num - 1; j++)
      job->commands[j] =
        build_command(cmd_args[j], dshell, middle_parser);

  } else {
    job->commands[0] =
      build_command(raw_args, dshell, only_parser);
  }

  for (int j = 0; j <= cmd_num; j++)
    free(cmd_args[j]);

  free(cmd_args);
  free(raw_args);
  return job;
}

int wait_for_process_group(pid_t pgid)
{
  int status;
  pid_t pid;
  int all_ok = 1;

  for (;;) {
    pid = waitpid(-pgid, &status, 0);

    if (pid > 0) {
      if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
          all_ok = -1;
        }
      } else if (WIFSIGNALED(status)) {
        all_ok = -1;
      }
    } else {
      if (errno == ECHILD) {
        break;
      } else {
        all_ok = -1;
        break;
      }
    }
  }

  return all_ok;
}

int launch_job(Job* job, Shell* dshell)
{
  if (!job || job->command_num == 0) return -1;

  for (int i = 0; i < job->command_num; i++) {
    if (job->command_num > 1 && job->commands[i]->parent_only) {
      print_error("Parent-only command cannot be used in a pipeline");
      return -1;
    }
  }

  int num_cmds = job->command_num;
  int num_pipes = (num_cmds > 1) ? (num_cmds - 1) : 0;

  int (*pipe_fds)[2] = NULL;
  if (num_pipes > 0) {
    pipe_fds = sf_malloc(sizeof(int[2]) * num_pipes);
    /* initialize to -1 for safe closing on cleanup */
    for (int p = 0; p < num_pipes; ++p) {
      pipe_fds[p][0] = -1;
      pipe_fds[p][1] = -1;
    }
  }

  /* create pipes */
  for (int p = 0; p < num_pipes; ++p) {
    if (pipe(pipe_fds[p]) == -1) {
      print_error("pipe creation failed");
      /* cleanup already-created pipes */
      for (int q = 0; q < p; ++q) {
        if (pipe_fds[q][0] != -1) close(pipe_fds[q][0]);
        if (pipe_fds[q][1] != -1) close(pipe_fds[q][1]);
      }
      free(pipe_fds);
      return -1;
    }
  }

  job->pgid = 0;
  int any_child_spawned = 0;

  for (int i = 0; i < num_cmds; i++) {
    Command* cmd = job->commands[i];

    if (!cmd) continue;

    /* assign the intended in/out fds for this command */
    if (!cmd->parent_only) {
      if (i != 0) cmd->in_fd = pipe_fds[i - 1][0];
      if (i != num_cmds - 1) cmd->out_fd = pipe_fds[i][1];
    }

    /* Launch command - pass pipe_fds so child can close unrelated fds */
    pid_t pid = launch_command(cmd, dshell, pipe_fds, num_pipes);

    if (pid < 0) {
      print_error("Failed to fork command");
      /* On error: close all pipe fds and free */
      if (pipe_fds) {
        for (int p = 0; p < num_pipes; ++p) {
          if (pipe_fds[p][0] != -1) { close(pipe_fds[p][0]); pipe_fds[p][0] = -1; }
          if (pipe_fds[p][1] != -1) { close(pipe_fds[p][1]); pipe_fds[p][1] = -1; }
        }
        free(pipe_fds);
      }
      return -1;
    }

    if (pid > 0) { /* parent: record that a real child was spawned */
      any_child_spawned = 1;

      if (job->pgid == 0) {
        job->pgid = pid;
        if (setpgid(pid, job->pgid) < 0 && errno != EACCES && errno != ESRCH) {
          print_error("parent: setpgid failed");
        }
      } else {
        if (setpgid(pid, job->pgid) < 0 && errno != EACCES && errno != ESRCH) {
          print_error("parent: join pgid failed");
        }
      }
    }

    /* Parent closes ends it doesn't need anymore (standard pipeline pattern) */
    if (i != 0) {
      /* after forking command i, parent no longer needs the read end of pipe i-1 */
      if (pipe_fds[i - 1][0] != -1) { close(pipe_fds[i - 1][0]); pipe_fds[i - 1][0] = -1; }
    }
    if (i != num_cmds - 1) {
      /* parent no longer needs the write end of pipe i */
      if (pipe_fds[i][1] != -1) { close(pipe_fds[i][1]); pipe_fds[i][1] = -1; }
    }
  }

  /* cleanup: ensure any remaining fds are closed */
  if (pipe_fds) {
    for (int p = 0; p < num_pipes; ++p) {
      if (pipe_fds[p][0] != -1) { close(pipe_fds[p][0]); pipe_fds[p][0] = -1; }
      if (pipe_fds[p][1] != -1) { close(pipe_fds[p][1]); pipe_fds[p][1] = -1; }
    }
    free(pipe_fds);
  }

  /* All commands were parent-only? */
  if (!any_child_spawned) {
    job->state = DONE;
    return 0;
  }

  if (!job->background) {
    if (job->pgid <= 0) {
      print_error("Invalid process group for job");
      return -1;
    }
    int status = wait_for_process_group(job->pgid);
    job->state = DONE;
    return status;
  }

  return 0;
}

void kill_job_list(Job** jobs, int jobs_num)
{
  if (!jobs) return;

  for (int i = 0; i < jobs_num; i++) {
    if (jobs[i]) {
      if (jobs[i]->pgid > 0) {
        kill(-jobs[i]->pgid, SIGKILL);
        waitpid(-jobs[i]->pgid, NULL, 0); 
      }
      free_job(jobs[i]);
    }
  }
  free(jobs);
}
