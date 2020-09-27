#include <stdio.h>

#include "./types.h"
#include "./parser.h"
#include "./eval.h"
#include "./repl.h"

int start_stdio_repl() {
  Value* result = NULL;
  char input[10000];

  ConsValue *env = init_global_env();

  while (1) {
    printf("\n> ");
    fflush(stdout);
    fgets(input, 10000, stdin);

    if (input[0] == ',') {
      if (input[1] == 'q') {
        puts("Quitting...");
        break;
      }
    } else {
      Value* form = parse_form(input);
      result = eval(form, env);
      print_value(result);
    }

    puts("");
  }

  return 0;
}
