#include <stdlib.h>
#include <string.h>

#include "./types.h"

SymbolValue* make_symbol(char *symbol_name) {
  SymbolValue* symbol = malloc(sizeof(SymbolValue));

  unsigned long str_size = strlen(symbol_name) + 1;
  char* symbol_name_copy = malloc(str_size);
  memcpy(symbol_name_copy, symbol_name, str_size);

  symbol->type = SymbolValueType;
  symbol->symbol_name = symbol_name;

  return symbol;
}
