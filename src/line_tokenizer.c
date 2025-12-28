#include "line_tokenizer.h"
#include <stdio.h>
#include "read_write.h"
#include <string.h>
#include <stdlib.h>
#include "args.h"

#define SPACE " \n\t\v\f\r"

char** tokenize_line(char* line) 
{
  char** tokens = init_args(BUFSIZ);
  int position = 0;

  for (char* token = strtok(line, SPACE);
       token != NULL;
       token = strtok(NULL, SPACE)) {

    if (position >= BUFSIZ) {
      print_error("Overflow bufsize");
      exit(1);
    }

    tokens[position++] = token;
  }

  return tokens;
}

