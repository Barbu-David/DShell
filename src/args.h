#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>
void free_args(char **args); 
void free_args_deep(char **args);
char** copy_args(char** args);
char** init_args(size_t size);

#endif 
