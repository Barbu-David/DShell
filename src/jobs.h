#ifndef JOBS_H
#define JOBS_H

#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>    
#include <string.h>

#include "commands.h"

typedef enum {
  RUNNING = 0,
  DONE,
  STOPPED
} JobState;

typedef struct Job{
  Command** commands;
  int command_num;
  int id;
  bool background;
  bool history;
  pid_t pgid;
  JobState state;
} Job;


Job* init_job(int commands_num);
Job* build_job(char** raw_args, Shell* dshell);
void copy_job(Job* src_job, Job* dst_job);
void free_job(Job* job);
void kill_job_list(Job** jobs, int jobs_num);

int launch_job(Job* job, Shell* dshell);

#endif 
