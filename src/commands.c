#include "commands.h"
#include "args.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void free_command(Command* c)
{
  free_args(c->args);
  c->infile=NULL;
  c->outfile=NULL;
  c->execute=NULL;
  free(c);
}

void free_command_deep(Command* c)
{
  free_args_deep(c->args);
  c->infile=NULL;
  c->outfile=NULL;
  c->execute=NULL;
  free(c);
}


Command* command_init(size_t size)
{
  
  Command* command = malloc(sizeof(Command));
  command->execute = NULL;
  command->infile=NULL;
  command->outfile=NULL;
  command->background = false;
  command->to_history = true;
  command->args = init_args(size);
  
  return command;
}

void copy_command(Command* src, Command* dst)
{
  if (!src || !dst) return;

  if (dst->args) {
    free_args_deep(dst->args);
    dst->args = NULL;
  }

  dst->background  = src->background;
  dst->to_history  = src->to_history;
  dst->execute     = src->execute;

  dst->args = copy_args(src->args);
}

