#include "./greatest.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "./checks.h"

#define check_eval(value, result_var, expected_type)  \
  result_var = eval(parse_form, NULL);                \
  ASSERT_EQ(expected_type, result_var->type);

TEST evaluates_numbers(void) {
  Value *result = parse_form("42");
  Value *num = eval(result, NULL);
  check_number(num, 42);

  PASS();
}

TEST evaluates_strings(void) {
  Value *result = parse_form("\"hello world!\"");
  Value *str = eval(result, NULL);
  check_string(str, "hello world!");

  PASS();
}

SUITE(eval_suite) {
    RUN_TEST(evaluates_numbers);
}
