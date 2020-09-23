#include <stdio.h>
#include "./types.h"
#include "./eval.h"

Value* eval(Value* expr, Value *env) {
  switch(expr->type) {
  case NumberValueType:
  case StringValueType:
    return expr;
  }

  // TODO: What's the ideal return value here?
  return NULL;
}
