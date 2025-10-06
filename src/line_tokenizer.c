#include "line_tokenizer.h"
#include <stdio.h>
#include "read_write.h"
#include <string.h>
#include <stdlib.h>

#define SPACE " \n\t\v\f\r"


char** tokenize_line(char* line) 
{
  char** tokens = malloc(BUFSIZ * sizeof(*tokens));
  if(tokens == NULL) {
    print_error("Malloc failed");
    exit(1);
  }

  int position=0;

  for(char* token = strtok(line, SPACE); token != NULL; token=strtok(NULL, SPACE)) {
    tokens[position++]=token;
      if(position>=BUFSIZ) {
        print_error("Overflow bufsize");
        exit(1);
      }
  }

  tokens[position]=NULL;

  return tokens;

}
