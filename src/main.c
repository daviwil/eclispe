#include <stdio.h>

#include "./types.h"
#include "./parser.h"

int main() {
  printf("Let's parse some Lisp.\n\n");

  Value* parse_result = parse_form("42");
  printf("Value type: %d\n", parse_result->type);

  return 0;
}
