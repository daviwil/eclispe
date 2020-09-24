#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "./types.h"

NumberValue* make_number(int number_value) {
  NumberValue* number = malloc(sizeof(NumberValue));

  number->type = NumberValueType;
  number->number_value = number_value;

  return number;
}

SymbolValue* make_symbol(char *symbol_name) {
  SymbolValue* symbol = malloc(sizeof(SymbolValue));

  unsigned long str_size = strlen(symbol_name) + 1;
  char* symbol_name_copy = malloc(str_size);
  memcpy(symbol_name_copy, symbol_name, str_size);

  symbol->type = SymbolValueType;
  symbol->symbol_name = symbol_name_copy;

  return symbol;
}

StringValue* make_string(char *string_value) {
  StringValue* string = malloc(sizeof(StringValue));

  unsigned long str_size = strlen(string_value) + 1;
  char* string_value_copy = malloc(str_size);
  memcpy(string_value_copy, string_value, str_size);

  string->type = StringValueType;
  string->string_value= string_value_copy;

  return string;
}

ConsValue* make_cons() {
  ConsValue* cons = malloc(sizeof(ConsValue));
  cons->type = ConsValueType;
  // TODO: Set nil in car and cdr

  return cons;
}

ConsValue* make_cons_with(Value* car, Value* cdr) {
  ConsValue *cons = make_cons();
  cons->car = car;
  cons->cdr = cdr;

  return cons;
}

ConsValue* make_list(unsigned int size, ...) {
  va_list argp;
  va_start(argp, size);

  ConsValue* cons = NULL;
  ConsValue* head = NULL;
  for (int i = 0; i < size; i++) {
    Value* item = va_arg(argp, Value*);
    if (item != NULL) {
      ConsValue* last_cons = cons;
      cons = make_cons();
      cons->car = item;

      if (last_cons != NULL) {
        last_cons->cdr = (Value*)cons;
      }

      if (head == NULL) {
        head = cons;
      }
    }
  }

  va_end(argp);

  return head;
}

void print_value(Value *value) {
  ConsValue* cons = (ConsValue*)value;

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

        if (cons->cdr != NULL) {
          print_value(cons->cdr);
        }
      }

      cons = NULL;
    }
    printf(")");
    break;
  default:
    printf("Unknown Value Type: %d\n", value->type);
    break;
  }
}
