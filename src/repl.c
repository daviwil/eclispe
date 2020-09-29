#include <stdio.h>
#include <string.h>

#include "./mem.h"
#include "./types.h"
#include "./parser.h"
#include "./error.h"
#include "./eval.h"
#include "./repl.h"

int start_stdio_repl() {
  Value* result = NULL;
  Error* error = NULL;
  char input[10000];

  ConsValue *env = init_global_env();

  while (1) {
    printf("> ");
    fflush(stdout);
    fgets(input, 10000, stdin);

    // Find first non-whitespace character
    int i = 0;
    for (; i < strlen(input); i++) {
      if (input[i] != ' ') {
        break;
      }
    }

    if (input[i] == ',') {
      if (input[i + 1] == 'q') {
        puts("Quitting...");
        break;
      } else if (input[i + 1] == 'm') {
        mem_print_status();
      }
    } else {
      Value* form = parse_form(input, &error);

      if (error) {
        printf("Error: %s\n", error->message);
        free_error(error);
        error = NULL;
      } else {

        result = eval(form, env, &error);

        free_value(form);

        if (error) {
          printf("Error: %s\n", error->message);
          free_error(error);
          error = NULL;
        } else {
          if (result == NULL) {
            printf("Error: No result, unknown error occurred!");
          } else {
            print_value(result);
            printf("\n");

            free_value(result);
          }
        }
      }
    }

    puts("");
  }

  return 0;
}
