#ifndef DSH_EXECUTE_H
#define DSH_EXECUTE_H

int dsh_execute(char** args, Shell* dshell);
void reap_background_process(Shell* dshell);
void free_args(char **args); 
int dsh_cd(char** args, Shell* dshell);
int dsh_help(char** args, Shell* dshell);
int dsh_exit(char** args, Shell* dshell);
int dsh_banner(char** args, Shell* dshell);
int dsh_history(char** args, Shell* dshell);

#endif 

