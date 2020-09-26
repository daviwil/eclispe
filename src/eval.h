#ifndef _EVAL_H
#define _EVAL_H

#include "./types.h"

Value* eval(Value* expression, ConsValue *environment);
ConsValue* init_global_env();

#endif
