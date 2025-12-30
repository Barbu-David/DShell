#ifndef DSHELL_H
#define DSHELL_H

#include <stdbool.h>

struct Builtin;
typedef struct Builtin Builtin;

struct Job;
typedef struct Job Job;

struct Command;
typedef struct Command Command;

#define MAX_JOBS 50

typedef struct Shell {
    bool running;
    Command* historyCommand;  

    Job** jobs;
    int curr_jobs;
    int job_capacity;   

    int num_builtins;
    Builtin* builtins;
} Shell;

Shell* shell_init();
void shell_step(Shell* dshell);
void shell_close(Shell* dshell);
void add_job(Shell* dshell, Job* job);
void remove_job(Shell* dshell, Job* job);

#endif 

