#include "jobs.h"
#include "sf_wraps.h"
#include "args.h"
#include "parser.h"
#include "read_write.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_PIPELINE_LENGTH 50

Job* init_job(int commands_num)
{
  Job* job = sf_malloc(sizeof(Job));
  job->commands = sf_malloc(sizeof(Command*) * commands_num); 
  job->command_num = commands_num;

  for (int k = 0; k < commands_num; k++) job->commands[k] = NULL;

  job->background = false;
  job->pgid = 0;
  job->state = RUNNING;
  return job;
}

void free_job(Job* job)
{
  for(int i = 0; i<job->command_num; i++) free_command(job->commands[i]);
  free(job->commands);
  free(job);
}

Job* build_job(char** raw_args, Shell* dshell)
{
  if (raw_args[0] == NULL) return NULL;

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
int launch_job(Job* job, Shell* dshell) 
{
    if (!job || job->command_num == 0) return -1;

    int num_cmds = job->command_num;
    int pipe_fds[num_cmds - 1][2];

    // Create the pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            print_error("pipe creation failed");
            return -1;
        }
    }

    // Assign infile/outfile for each command (use FD: marker)
    for (int i = 0; i < num_cmds; i++) {
        Command* cmd = job->commands[i];

        if (i != 0) {
            char buf[32];
            snprintf(buf, sizeof(buf), "FD:%d", pipe_fds[i - 1][0]);
            cmd->infile = sf_strdup(buf);
        }

        if (i != num_cmds - 1) {
            char buf[32];
            snprintf(buf, sizeof(buf), "FD:%d", pipe_fds[i][1]);
            cmd->outfile = sf_strdup(buf);
        }
    }

    // Execute all commands sequentially but properly close pipe fds in parent
    for (int i = 0; i < num_cmds; i++) {
        Command* cmd = job->commands[i];
        if (cmd->execute) {
            cmd->execute(cmd, dshell);   // this will fork for external commands
        }

        // Parent cleanup to avoid holding pipe ends open and causing readers to block:
        // After launching cmd i:
        //  - close current write end (pipe_fds[i][1]) in parent so readers see EOF when writers done
        //  - close previous read end (pipe_fds[i-1][0]) because the reader (i) inherited it
        if (i != num_cmds - 1) {
            // close write end of the *current* pipe in parent
            close(pipe_fds[i][1]);
        }
        if (i != 0) {
            // close read end of the *previous* pipe in parent (reader has been forked)
            close(pipe_fds[i - 1][0]);
        }
    }

    // Any remaining fds (should be none) — but be defensive:
    for (int i = 0; i < num_cmds - 1; i++) {
        // these may already be closed; ignore errors
        close(pipe_fds[i][0]);
        close(pipe_fds[i][1]);
    }

    free_job(job);

    return 0;
}

