#include <stdlib.h>

#include "dshell.h"
#include "ui.h"
#include "read_write.h"
#include "line_tokenizer.h"
#include "builtins.h"
#include "args.h"
#include "sf_wraps.h"

Shell* shell_init() {

    print_banner(PURPLE);

    Shell* dshell = (Shell*) sf_malloc(sizeof(Shell));

    dshell->historyCommand = init_command();
    dshell->running = true;
    dshell->curr_jobs=0;
    dshell->job_capacity=50;
   
    dshell->jobs= (Job**) sf_malloc(sizeof(Job*) * dshell->job_capacity);
    
    dshell->builtins=init_builtins(&(dshell->num_builtins));

    return dshell;
}

void shell_close(Shell* dshell) 
{
  free_command(dshell->historyCommand);
  free(dshell->jobs);
  free(dshell);
}

void shell_step(Shell* dshell) 
{
  print_shell_prompt();

  char* line = read_line();
  char** args = tokenize_line(line);
  Job* job = build_job(args, dshell);

  add_job(dshell, job);
  
  int status = launch_job(job, dshell);

  if(status == -1) print_error("Failed to execute program");

  free(line);
}

void add_job(Shell* dshell, Job* job)
{

  if(dshell->curr_jobs >= dshell->job_capacity) return; // TO DO add reallocation
  job->id=dshell->curr_jobs;
  for(int i=0; i<job->command_num; i++) job->commands[i]->job_id=job->id;

  dshell->jobs[dshell->curr_jobs++]=job;

}

void remove_job(Shell* dshell, Job* job)
{
    if (!dshell || !job) return;
    int idx = job->id;

    if (idx < 0 || idx >= dshell->curr_jobs) return;

    dshell->curr_jobs--; 
    dshell->jobs[idx] = dshell->jobs[dshell->curr_jobs]; 
    if (dshell->jobs[idx]) dshell->jobs[idx]->id = idx;    
    dshell->jobs[dshell->curr_jobs] = NULL;
}

