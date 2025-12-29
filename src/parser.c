#include <stdlib.h>
#include <string.h>

#include "read_write.h"
#include "commands.h"
#include "args.h"
#include "sf_wraps.h"
#include "parser.h"

void first_parser(Command* command, int count, char** args)
{

  char** tmp_args = init_args(count);
  int j = 0;

  for (int i = 0; args[i]; i++) {

    if (strcmp(args[i], "<") == 0) {
      if (!args[i+1]) { print_error("Syntax error: no input file"); }
      command->infile = sf_strdup(args[++i]);
    }
    else if (strcmp(args[i], ">") == 0) {
      if (args[i+1]) i++;
      print_error("Can't redirect output at the beggining of the pipeline. Output stream ignored");
    }
    else if (strcmp(args[i], "&") == 0) {
      print_error("Can only use & at the end");
    }
    else {
      tmp_args[j++] = args[i];
    }
  }

  command->args = copy_args(tmp_args);

  free(tmp_args); 
}

void only_parser(Command* command, int count, char** args)
{

  char** tmp_args = init_args(count);
  int j = 0;

  for (int i = 0; args[i]; i++) {

    if (strcmp(args[i], "<") == 0) {
      if (!args[i+1]) { print_error("Syntax error: no input stream"); }
      command->infile = sf_strdup(args[++i]);
    }
    else if (strcmp(args[i], ">") == 0) {
      if (!args[i+1]) { print_error("Syntax error: no output stream"); }
      command->outfile = sf_strdup(args[++i]);
    }
    else if (strcmp(args[i], "&") == 0) {
      if(!args[i+1]) print_error("Can only use & at the end");
    }
    else {
      tmp_args[j++] = args[i];
    }
  }

  command->args = copy_args(tmp_args);

  free(tmp_args); 

}

void middle_parser(Command* command, int count, char** args)
{

  char** tmp_args = init_args(count);
  int j = 0;

  for (int i = 0; args[i]; i++) {

    if (strcmp(args[i], "<") == 0) {
       if (args[i+1]) i++;
      print_error("Can't redirect input in the middle of the pipeline. Intput stream ignored");
    
    }
    else if (strcmp(args[i], ">") == 0) {
      if (args[i+1]) i++;
      print_error("Can't redirect output in the middle of the pipeline. Output stream ignored");
    }
    else if (strcmp(args[i], "&") == 0) {
      print_error("Can only use & at the end");
    }
    else {
      tmp_args[j++] = args[i];
    }
  }

  command->args = copy_args(tmp_args);

  free(tmp_args); 

}

void final_parser(Command* command, int count, char** args)
{
  char** tmp_args = init_args(count);
  int j = 0;

  for (int i = 0; args[i]; i++) {

    if (strcmp(args[i], "<") == 0) {
        if (args[i+1]) i++;
      print_error("Can't redirect input at the end of the pipeline. Intput stream ignored");
       
    }
    else if (strcmp(args[i], ">") == 0) {
      if (!args[i+1]) { print_error("Syntax error: no output stream"); }
      command->outfile = sf_strdup(args[++i]);
    }
    else if (strcmp(args[i], "&") == 0) {

      if(!args[i+1]) print_error("Can only use & at the end");
    }
    else {
      tmp_args[j++] = args[i];
    }
  }

  command->args = copy_args(tmp_args);

  free(tmp_args); 

}


