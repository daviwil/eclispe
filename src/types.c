#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

ConsValue* make_cons() {
  ConsValue* cons = malloc(sizeof(ConsValue));
  cons->type = ConsValueType;
  // TODO: Set nil in car and cdr

  return cons;
}

void print_value(Value *value) {
  ConsValue* cons = (ConsValue*)value;

  // TODO: Assert not null?
  switch (value->type) {
  case NumberValueType:
    printf("%d", ((NumberValue *) value)->number_value);
    break;
  case SymbolValueType:
    printf("%s", ((SymbolValue *) value)->symbol_name);
    break;
  case ConsValueType:
    printf("(");
    while (cons != NULL) {
      print_value(cons->car);
      if (cons->cdr) {
        if (cons->cdr->type != ConsValueType) {
          printf(" . ");
          cons = (ConsValue*)cons->cdr;
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
