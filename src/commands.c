#include "commands.h"
#include "args.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void free_command(Command* c)
{
  free_args(c->args);
  if(c->infile) free(c->infile);
  if(c->outfile) free(c->outfile);
  c->execute=NULL;
  free(c);
}

Command* init_command()
{
  
  Command* command = malloc(sizeof(Command));
  command->execute = NULL;
  command->infile=NULL;
  command->outfile=NULL;
  command->background = false;
  command->to_history = true;
  command->args = NULL;

  return command;
}

void copy_command(Command* src, Command* dst)
{
  if (!src || !dst) return;

  if (dst->args) {
    free_args(dst->args);
    dst->args = NULL;
  }

  dst->background  = src->background;
  dst->to_history  = src->to_history;
  dst->execute     = src->execute;

  dst->args = copy_args(src->args);
}

