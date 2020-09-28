#ifndef _LOG_H
#define _LOG_H

#include <string.h>
#include "./types.h"

enum LogLevel {
  LogNone,
  LogParse,
  LogEval,
  LogAll
};

typedef unsigned char LogState;

void log_set_scope(const char* scope);
void log_format(const char* format, ...);
void log_value(const char* message, Value* value);

#endif
