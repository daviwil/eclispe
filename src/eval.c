#include <stdio.h>
#include <string.h>

#include "./types.h"
#include "./list.h"
#include "./eval.h"
#include "./log.h"

#define is_pair(value) (value->type == ConsValueType)
#define is_atom(value) !is_pair(value)

Value* cons_car(ConsValue* cons) {
  if (cons->type == ConsValueType) {
    return cons->car;
  }

  return NULL;
}

Value* cons_set_car(ConsValue* cons, Value* new_value) {
  if (cons->type == ConsValueType) {
    cons->car = new_value;
    return new_value;
  }

  return NULL;
}

Value* cons_cdr(ConsValue* cons) {
  if (cons->type == ConsValueType) {
    return cons->cdr;
  }

  return NULL;
}

Value* cons_set_cdr(ConsValue* cons, Value* new_value) {
  if (cons->type == ConsValueType) {
    cons->cdr = new_value;
    return new_value;
  }

  return NULL;
}

Value* cons_nth(ConsValue *cons, int n) {
  for (int i = 0; i < n; i++) {
    if (cons->type != ConsValueType) {
      // TODO: Error?
      return NULL;
    }

    cons = (ConsValue*)cons_cdr(cons);
    if (cons == NULL) {
      return NULL;
    }
  }

  return (Value*) cons;
}

Value* cons_cadr(ConsValue* cons) {
  // TODO: Ensure result
  return cons_car((ConsValue*)cons_nth(cons, 1));
}

Value* cons_cddr(ConsValue* cons) {
  return cons_nth(cons, 2);
}

Value* cons_caddr(ConsValue* cons) {
  // TODO: Ensure result
  return cons_car((ConsValue*)cons_nth(cons, 2));
}

Value* cons_cadddr(ConsValue* cons) {
  // TODO: Ensure result
  return cons_car((ConsValue*)cons_nth(cons, 3));
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

Value* eprogn(ConsValue* cons, ConsValue* env) {
  log_value("eprogn: ", (Value*)cons);
  if (env) {
    log_value("  env: ", (Value*)cons);
  }

  // This may need to change to be more like the LISP implementation
  Value* result = NULL;
  while (cons) {
    result = eval(cons_car(cons), env);
    cons = (ConsValue*)cons_cdr(cons);
    // TODO: Make sure it's really a cons
  }

  log_value("  result: ", result);

  return result;
}

Value* invoke(FunctionValue* func, ConsValue *args) {
  log_set_scope("eval:invoke");

  log_value("Invoke func: ", (Value*)func);
  log_value("Invoke args: ", (Value*)args);

  // For each arg, match it with params
  ConsValue* params = func->args;

  log_value("Expected parameters: ", (Value*)params);

  // TODO: Use push_list_item
  ConsValue* env_head = NULL;
  ConsValue* env = NULL;
  while (params != NULL) {
    ConsValue* pair = make_cons_with(params->car, args->car);
    ConsValue *new_arg = make_cons_with((Value*)pair, NULL);
    if (env_head == NULL) {
      env = env_head = new_arg;
    } else {
      env->cdr = (Value*)new_arg;
      env = new_arg;
    }

    params = (ConsValue*)params->cdr;
    args = (ConsValue*)args->cdr;
  }

  // Attach the definition environment
  env->cdr = (Value*)func->env;

  log_value("  env:", (Value*) env_head);

  if (func->invoker != NULL) {
    log_format("Invoking primitive");
    return func->invoker(env_head);
  } else {
    log_format("Invoking body");
    return eprogn(func->body, env_head);
  }
}

ConsValue* evlis(ConsValue* exprs, ConsValue* env) {
  ConsValue* cons = NULL;
  ConsValue* head = NULL;

  log_format("Evaluating arguments");

  // Evaluate each item and return a new list
  while (exprs != NULL) {
    if (exprs->type != ConsValueType) {
      return NULL;
    }

    ConsValue* last_cons = cons;
    cons = make_cons();
    if (head == NULL) {
      head = cons;
    }

    cons_set_car(cons, eval(exprs->car, env));
    if (last_cons != NULL) {
      last_cons->cdr = (Value*)cons;
    }

    exprs = (ConsValue*)cons_cdr(exprs);
  }

  log_format("Done evaluating arguments");

  return (ConsValue*) head;
}

Value* lookup(SymbolValue *symbol, ConsValue* env) {
  ConsValue* cons = (ConsValue*) env;
  while (cons != NULL) {
    if (cons->type != ConsValueType) {
      return NULL;
    }

    Value* pair_symbol = cons_car((ConsValue*)cons->car);
    if (equal(pair_symbol, (Value*)symbol)) {
      return cons_cdr((ConsValue*)cons->car);
    }

    cons = (ConsValue*)cons->cdr;
  }

  return NULL;
}

Value *lookup_by_name(char *name, ConsValue *env) {
  SymbolValue symbol;
  symbol.type = SymbolValueType;
  symbol.symbol_name = name;
  return lookup(&symbol, env);
}

Value* update(Value* symbol, Value* new_value, ConsValue* env) {
  ConsValue* cons = env;
  while (cons != NULL) {
    if (cons->type != ConsValueType) {
      return NULL;
    }

    Value* pair_symbol = cons_car((ConsValue*)cons->car);
    if (equal(pair_symbol, (Value*)symbol)) {
      // TODO: Create a new cons cell for the new value?
      return cons_set_cdr((ConsValue*)cons->car, new_value);
    }

    cons = (ConsValue*)cons->cdr;
  }

  return NULL;
}

Value* prim_add(ConsValue* env) {
  Value* x = lookup_by_name("x", env);
  Value* y = lookup_by_name("y", env);

  if (x && x->type == NumberValueType) {
    if (y) {
      if (y->type == NumberValueType) {
        return (Value*)make_number(((NumberValue*)x)->number_value + ((NumberValue*)y)->number_value);
      } else {
        puts("Argument 'y' is not a number!");
      }
    } else {
      return x;
    }
  } else {
    puts("Argument 'x' is not a number!");
  }

  return NULL;
}

Value* bind_prim(char* name, PrimFunc invoker, ConsValue* args) {
  return (Value*)make_cons_with((Value*)make_symbol(name), (Value*)make_prim_function(invoker, args, NULL));
}

ConsValue* init_global_env() {
  ConsList env_list;
  env_list.head = NULL;
  env_list.tail = NULL;

  log_format("Initializing global environment");

  // Add primitive functions
  push_list_item(&env_list, bind_prim("+", &prim_add, make_list(2, make_symbol("x"), make_symbol("y"))));

  log_value("Initial environment: ", (Value*)env_list.head);

  return env_list.head;
}

Value* eval(Value* value, ConsValue* env) {
  log_set_scope("eval");

  if (is_atom(value)) {
    switch(value->type) {
    case NumberValueType:
    case StringValueType:
      return value;
    case SymbolValueType:
      return lookup((SymbolValue*)value, env);
    }
  } else {
    ConsValue* expr = (ConsValue*)value;
    Value* operator = cons_car((ConsValue*)expr);
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
        // TODO: Verify cons
        return eprogn((ConsValue*)cons_cdr(expr), env);
      } else if (is_symbol_named(operator, "lambda")) {
        Value* args = cons_cadr(expr);
        Value* body = cons_cddr(expr);
        if (args->type != ConsValueType) {
          puts("Lambda must have a list in argument position");
          return NULL;
        }
        if (body == NULL) {
          puts("Lambda must have a body");
          return NULL;
        }
        return (Value*)make_function((ConsValue*)body, (ConsValue*)args, (ConsValue*)env);
      } else {
        FunctionValue* func = (FunctionValue*)eval(operator, env);
        log_value("Symbol operator: ", operator);
        if (func) {
          if (func->type == FunctionValueType) {
            return invoke(func, evlis((ConsValue*)cons_cdr(expr), env));
          } else {
            puts("Cannot eval non-function in operator position!");
            return NULL;
          }
        } else {
          printf("Could not resolve operator: ");
          print_value(operator);
          puts("");
        }
      }
    } else {
      FunctionValue* func = (FunctionValue*)eval(operator, env);
      if (func->type == FunctionValueType) {
        log_value("Lambda operator: ", operator);
        return invoke(func, evlis((ConsValue*)cons_cdr(expr), env));
      } else {
        puts("Cannot eval non-function in operator position!");
        return NULL;
      }

      printf("Operator is not a symbol!\n");
    }
  }

  printf("Exiting eval without a result.\n");

  // TODO: What's the ideal return value here?
  return NULL;
}
