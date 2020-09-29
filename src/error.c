#include "./mem.h"
#include "./error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Error* make_error(const char* message) {
  Error* error = (Error*)mem_alloc(sizeof(Error));

  unsigned long str_size = strlen(message) + 1;
  char* message_copy = (char *)mem_alloc(str_size);
  memcpy(message_copy, message, str_size);

  error->message = message_copy;

  return error;
}

void free_error(Error* error) {
  if (error) {
    if (error->location) {
      // TODO: Free location
    }

    mem_free(error->message);
    mem_free(error);
  }
}
