#include <stdio.h>
#include <stdarg.h>

#include "./types.h"
#include "./log.h"

LogState LOG_ACTIVE = 0; // TODO: Default to 0
char LOG_SCOPE[500] = "msh";
char* LOG_FILTER = NULL;

#define SHOULD_LOG() (LOG_ACTIVE && (!LOG_FILTER || strncmp(LOG_SCOPE, LOG_FILTER, strlen(LOG_FILTER)) == 0))

void log_set_scope(const char* scope) {
  sprintf(LOG_SCOPE, "%s", scope);
}

void log_format(const char* format, ...) {
  if (SHOULD_LOG()) {
    va_list args;
    printf("%s | ", LOG_SCOPE);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    puts("");
  }
}

void log_value(const char* message, Value* value) {
  if (SHOULD_LOG()) {
    printf("%s | %s", LOG_SCOPE, message);
    print_value(value);
    puts("");
  }
}
