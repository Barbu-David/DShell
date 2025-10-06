#ifndef DSH_EXECUTE_H
#define DSH_EXECUTE_H

int dsh_execute(char** args);
int reap_background_process(int background_process);

int dsh_cd(char** args);
int dsh_help(char** args);
int dsh_exit(char** args);
int dsh_banner(char** args);

#endif 

