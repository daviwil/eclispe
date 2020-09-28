#ifndef _ERROR_H
#define _ERROR_H

typedef struct {
  char* file_path;
  unsigned int line;
  unsigned int col;
} SourceLocation;

typedef struct ErrorStruct {
  char* message;
  SourceLocation* location;
  struct ErrorStruct* inner_error;
} Error;

Error* make_error(const char* message);
void free_error(Error* error);

#endif
