#include "./types.h"
#include "./parser.h"

Value* parse_form(char *form_string) {
  return (Value *)make_symbol("car");
}
