#include "./greatest.h"
#include "../src/list.h"
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

TEST evaluates_if(void) {
  Value *result = parse_form("(if 1 2 3)");
  Value *num = eval(result, NULL);
  check_number(num, 2);

  PASS();
}

TEST evaluates_quote(void) {
  Value *result = parse_form("(quote hamburger)");
  Value *symbol = eval(result, NULL);
  check_symbol(symbol, "hamburger");

  PASS();
}

TEST evaluates_begin(void) {
  Value *result = parse_form("(begin 1 2 3)");
  Value *num = eval(result, NULL);
  check_number(num, 3);

  PASS();
}

TEST evaluates_symbol(void) {
  ConsValue *env = make_list(2, make_cons_with((Value*)make_symbol("bogus"), (Value*)make_number(2)),
                                make_cons_with((Value*)make_symbol("test"),  (Value*)make_number(4)));

  Value *result = parse_form("test");
  Value *num = eval(result, env);
  check_number(num, 4);

  result = parse_form("bork");
  Value *nope = eval(result, env);
  ASSERT_EQ(NULL, nope);

  PASS();
}

TEST evaluates_set(void) {
  ConsValue *env = make_list(1, make_cons_with((Value*)make_symbol("test"),  (Value*)make_number(4)));

  Value* result = parse_form("(set! test 311)");
  Value* num = eval(result, env);
  check_number(num, 311);

  result = parse_form("test");
  Value* new_num = eval(result, env);
  check_number(num, 311);

  result = parse_form("(set! bogus \"oh no\")");
  Value* nope = eval(result, env);
  ASSERT_EQ(NULL, nope);

  PASS();
}

TEST evaluates_lambda(void) {
  ConsValue *env = init_global_env();

  Value* result = parse_form("((lambda (x) x) 2)");
  print_value(result);
  puts("");
  Value* num = eval(result, env);
  check_number(num, 2);

  result = parse_form("((lambda (x) ((lambda (y) (+ x y)) 3)) 2)");
  num = eval(result, env);
  check_number(num, 5);

  PASS();
}

SUITE(eval_suite) {
  RUN_TEST(evaluates_numbers);
  RUN_TEST(evaluates_strings);
  RUN_TEST(evaluates_if);
  RUN_TEST(evaluates_quote);
  RUN_TEST(evaluates_begin);
  RUN_TEST(evaluates_symbol);
  RUN_TEST(evaluates_set);
  RUN_TEST(evaluates_lambda);
}
