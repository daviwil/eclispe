#include "../src/mem.h"
#include "./greatest.h"
#include "./parser_test.h"
#include "./eval_test.h"

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(parser_suite);
  RUN_SUITE(eval_suite);

  puts("");

  mem_print_status();

  GREATEST_MAIN_END();
}
