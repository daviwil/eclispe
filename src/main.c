#include <stdio.h>
#include "./repl.h"

int main() {
  printf("Let's parse some Lisp.\n");

  return start_stdio_repl();
}
