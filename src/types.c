#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./log.h"
#include "./mem.h"
#include "./types.h"

NumberValue* make_number(int number_value) {
  NumberValue* number = mem_alloc(sizeof(NumberValue));

  number->type = NumberValueType;
  number->number_value = number_value;

  return number;
}

SymbolValue* make_symbol(char *symbol_name) {
  SymbolValue* symbol = mem_alloc(sizeof(SymbolValue));

  unsigned long str_size = strlen(symbol_name) + 1;
  char* symbol_name_copy = mem_alloc(str_size);
  memcpy(symbol_name_copy, symbol_name, str_size);

  symbol->type = SymbolValueType;
  symbol->symbol_name = symbol_name_copy;

  return symbol;
}

void free_symbol(SymbolValue* symbol) {
  mem_free(symbol->symbol_name);
  mem_free(symbol);
}

StringValue* make_string(char *string_value) {
  StringValue* string = mem_alloc(sizeof(StringValue));

  unsigned long str_size = strlen(string_value) + 1;
  char* string_value_copy = mem_alloc(str_size);
  memcpy(string_value_copy, string_value, str_size);

  string->type = StringValueType;
  string->string_value= string_value_copy;

  return string;
}

void free_string(StringValue* string) {
  mem_free(string->string_value);
  mem_free(string);
}

ConsValue* make_cons() {
  ConsValue* cons = mem_alloc(sizeof(ConsValue));
  cons->type = ConsValueType;
  cons->car = NULL;
  cons->cdr = NULL;

  // TODO: Set nil in car and cdr

  return cons;
}

ConsValue* make_cons_with(Value* car, Value* cdr) {
  ConsValue *cons = make_cons();
  cons->car = car;
  cons->cdr = cdr;

  return cons;
}

void free_cons(ConsValue *cons) {
  if (cons->car) {
    free_value(cons->car);
    cons->car = NULL;
  }
  if (cons->cdr) {
    free_value(cons->cdr);
    cons->cdr = NULL;
  }

  mem_free(cons);
}

FunctionValue* make_function(ConsValue* body, ConsValue* args, ConsValue* env) {
  FunctionValue* func = mem_alloc(sizeof(FunctionValue));
  func->type = FunctionValueType;
  func->args = args;
  func->body = body;
  func->env = env;

  return func;
}

FunctionValue* make_prim_function(PrimFunc invoker, ConsValue* args, ConsValue* env) {
  FunctionValue* func = make_function(NULL, args, env);
  func->invoker = invoker;

  return func;
}

void free_value(Value *value) {
  log_value("Freeing value: ", value);

  if (value) {
    switch (value->type) {
    case NumberValueType:
      mem_free(value);
      break;

    case StringValueType:
      free_string((StringValue*)value);
      break;

    case SymbolValueType:
      free_symbol((SymbolValue*)value);
      break;

    case ConsValueType:
      free_cons((ConsValue*)value);
      break;

    default:
      log("Attempted to free unknown value type: %d", value->type);
    }
  } else {
    log("Attempted to free NULL value");
  }
}

void print_value(Value *value) {
  ConsValue* cons = (ConsValue*)value;
  FunctionValue* func = (FunctionValue*)value;

  // TODO: Assert not null?
  switch (value->type) {
  case NumberValueType:
    printf("%d", ((NumberValue*)value)->number_value);
    break;

  case SymbolValueType:
    printf("%s", ((SymbolValue*)value)->symbol_name);
    break;

  case StringValueType:
    printf("\"%s\"", ((StringValue*)value)->string_value);
    break;

  case ConsValueType:
    printf("(");
    while (cons != NULL) {
      print_value(cons->car);
      if (cons->cdr) {
        if (cons->cdr->type != ConsValueType) {
          printf(" . ");
        } else {
          printf(" ");
        }

        fflush(stdout);

        if (cons->cdr != NULL) {
          print_value(cons->cdr);
        }
      }

      cons = NULL;
    }
    printf(")");
    break;

  case FunctionValueType:
    if (func->invoker) {
      printf("(prim ");
    } else {
      printf("(lambda ");
    }

    /* print_value(func->args); */
    printf(")");
    break;

  default:
    printf("Unknown Value Type: %d\n", value->type);
    break;
  }
}
