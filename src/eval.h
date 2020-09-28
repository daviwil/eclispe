#ifndef _EVAL_H
#define _EVAL_H

#include "./types.h"
#include "./error.h"

Value* eval(Value* value, ConsValue* env, Error** error);
ConsValue* init_global_env();

#endif
