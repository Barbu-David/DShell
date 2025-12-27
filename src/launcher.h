#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stdbool.h>
#include <fcntl.h>    
#include <sys/stat.h>

int launch_task(char** args,
                const char *infile,
                const char *outfile,
                bool blocking);

#endif 

