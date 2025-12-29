#include "jobs.h"
#include "sf_wraps.h"
#include <stdlib.h>
Job* init_job(int commands_num)
{
  Job* job = sf_malloc(sizeof(Job)*commands_num);
  job->commands = sf_malloc(sizeof(Command*));
  job->command_num=commands_num;
  job->background=false;
  job->pgid=0;
  job->state=RUNNING;
  return job;
}

void clean_job(Job* job)
{
  for(int i = 0; i<job->command_num; i++) free_command(job->commands[i]);
  free(job->commands);
  free(job);
}
