#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

void free_args(char **args);
char** init_args(size_t size);
char** copy_args(char **args);

#endif 
