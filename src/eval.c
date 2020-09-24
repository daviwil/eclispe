#include <stdio.h>
#include <string.h>
#include "./types.h"
#include "./eval.h"

#define is_pair(value) (value->type == ConsValueType)
#define is_atom(value) !is_pair(value)

Value* cons_car(Value* cons) {
  if (cons->type == ConsValueType) {
    return ((ConsValue *)cons)->car;
  }

  return NULL;
}

Value* cons_cdr(Value* cons) {
  if (cons->type == ConsValueType) {
    return ((ConsValue *)cons)->cdr;
  }

  return NULL;
}

Value* cons_set_cdr(Value* cons, Value* new_value) {
  if (cons->type == ConsValueType) {
    ((ConsValue*)cons)->cdr = new_value;
    return new_value;
  }

  return NULL;
}

Value* cons_nth(Value *cons, int n) {
  for (int i = 0; i < n; i++) {
    cons = cons_cdr(cons);
    if (cons == NULL) {
      return NULL;
    }
  }

  return cons;
}

Value* cons_cadr(Value* cons) {
  return cons_car(cons_nth(cons, 1));
}

Value* cons_cddr(Value* cons) {
  return cons_nth(cons, 2);
}

Value* cons_caddr(Value* cons) {
  return cons_car(cons_nth(cons, 2));
}

Value* cons_cadddr(Value* cons) {
  return cons_car(cons_nth(cons, 3));
}

int is_symbol_named(Value* symbol, char* expected_name) {
  if (symbol->type == SymbolValueType) {
    return strcmp(((SymbolValue*)symbol)->symbol_name, expected_name) == 0;
  }

  return 0;
}

int equal(Value* left, Value* right) {
  // Same object
  if (left == right) {
    return 1;
  }

  if (left != NULL && right != NULL) {
    if (left->type == right->type) {
      switch(left->type) {
      case NumberValueType:
        return ((NumberValue*)left)->number_value == ((NumberValue*)right)->number_value;
      case StringValueType:
        return strcmp(((StringValue*)left)->string_value, ((StringValue*)right)->string_value) == 0;
      case SymbolValueType:
        return strcmp(((SymbolValue*)left)->symbol_name, ((SymbolValue*)right)->symbol_name) == 0;
      default:
        printf("Don't know how to compare values of type %d!", left->type);
      }
    }
  }

  return 0;
}

Value* eprogn(Value* cons, Value* env) {
  // This may need to change to be more like the LISP implementation
  Value* result = NULL;
  while (cons) {
    result = eval(cons_car(cons), env);
    cons = cons_cdr(cons);
  }

  return result;
}

Value* evlis(Value* exprs, Value* env) {
  return NULL;
}

Value* lookup(SymbolValue *symbol, Value* env) {
  ConsValue* cons = (ConsValue*) env;
  while (cons != NULL) {
    if (cons->type != ConsValueType) {
      return NULL;
    }

    Value* pair_symbol = cons_car(cons->car);
    if (equal(pair_symbol, (Value*)symbol)) {
      return cons_cdr(cons->car);
    }

    cons = (ConsValue*)cons->cdr;
  }

  return NULL;
}

Value* update(Value* symbol, Value* new_value, Value* env) {
  ConsValue* cons = (ConsValue*) env;
  while (cons != NULL) {
    if (cons->type != ConsValueType) {
      return NULL;
    }

    Value* pair_symbol = cons_car(cons->car);
    if (equal(pair_symbol, (Value*)symbol)) {
      // TODO: Create a new cons cell for the new value?
      return cons_set_cdr(cons->car, new_value);
    }

    cons = (ConsValue*)cons->cdr;
  }

  return NULL;
}

Value* make_function(Value* args, Value* body, Value* env) {
  return NULL;
}

Value* eval(Value* expr, Value* env) {
  if (is_atom(expr)) {
    switch(expr->type) {
    case NumberValueType:
    case StringValueType:
      return expr;
    case SymbolValueType:
      return lookup((SymbolValue*)expr, env);
    }
  } else {
    Value* operator = cons_car(expr);
    if (operator->type == SymbolValueType) {
      // Check for special forms
      if (is_symbol_named(operator, "if")) {
        // TODO: Check argument length
        Value* if_result = eval(cons_cadr(expr), env);

        // TODO: Check for nil and false
        if (if_result) {
          return eval(cons_caddr(expr), env);
        } else {
          return eval(cons_cadddr(expr), env);
        }
      } else if (is_symbol_named(operator, "set!")) {
        return update(cons_cadr(expr), eval(cons_caddr(expr), env), env);
      } else if (is_symbol_named(operator, "quote")) {
        return cons_cadr(expr);
      } else if (is_symbol_named(operator, "begin")) {
        // TODO: Use environment
        return eprogn(cons_cdr(expr), NULL);
      } else if (is_symbol_named(operator, "lambda")) {
        return make_function(cons_cadr(expr), cons_cddr(expr), env);
      }
    } else {
      printf("Operator is not a symbol!\n");
    }
  }

  printf("Exiting eval without a result.\n");

  // TODO: What's the ideal return value here?
  return NULL;
}
