#ifndef _LOG_H
#define _LOG_H

#include <string.h>
#include "./types.h"

typedef enum {
  LogNone,
  LogNormal,
  LogVerbose,
} LogLevel;

typedef unsigned char LogState;

void log_set_scope(const char* scope);
void log_format(LogLevel level, const char* format, ...);
void log_value(const char* message, Value* value);

#define log(format, ...) log_format(LogNormal, format __VA_OPT__(,) __VA_ARGS__)
#define log_verbose(format, ...) log_format(LogVerbose, format __VA_OPT__(,) __VA_ARGS__)

#endif
