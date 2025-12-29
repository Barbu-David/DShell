#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "sf_wraps.h"
#include "read_write.h"

void* sf_malloc(size_t size)
{
  void* ptr = malloc(size);

  if(ptr == NULL) {
    print_error("Malloc failed, exiting program");
    if(errno) print_error(strerror(errno));
    exit(1);
  }

  return ptr;
}


char* sf_strdup(char* str) {

  char* dup = strdup(str);

  if(dup == NULL) {
    print_error("strdup failed, exiting program");
    if(errno) print_error(strerror(errno));
    exit(1);
  }

  return dup;

}

