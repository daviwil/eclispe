#include <stdio.h>
#include "./repl.h"

int main() {
  printf("\033[1;32m\neclispe REPL\033[0m\n");
  printf("Type ,q to quit\n\n");

  return start_stdio_repl();
}
