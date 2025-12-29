#include <stdlib.h>

#include "commands.h"
#include "args.h"
#include "sf_wraps.h"

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
  
  Command* command = (Command*) sf_malloc(sizeof(Command));
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

  if(src->infile) dst->infile = sf_strdup(src->infile);
  if(src->outfile) dst->outfile = sf_strdup(src->outfile);

  dst->args = copy_args(src->args);
}

