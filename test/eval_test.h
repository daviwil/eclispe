#include "./greatest.h"
#include "../src/list.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "./checks.h"

TEST evaluates_numbers(void) {
  Error* error = NULL;
  Value *result = parse_form("42", &error);
  ASSERT_EQ(NULL, error);

  Value *num = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 42);

  PASS();
}

TEST evaluates_strings(void) {
  Error* error = NULL;
  Value *result = parse_form("\"hello world!\"", &error);
  Value *str = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_string(str, "hello world!");

  PASS();
}

TEST evaluates_if(void) {
  Error* error = NULL;
  Value *result = parse_form("(if 1 2 3)", &error);
  Value *num = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 2);

  PASS();
}

TEST evaluates_quote(void) {
  Error* error = NULL;
  Value *result = parse_form("(quote hamburger)", &error);
  Value *symbol = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_symbol(symbol, "hamburger");

  PASS();
}

TEST evaluates_begin(void) {
  Error* error = NULL;
  Value *result = parse_form("(begin 1 2 3)", &error);
  Value *num = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 3);

  PASS();
}

TEST evaluates_symbol(void) {
  Error* error = NULL;
  ConsValue *env = make_list(2, make_cons_with((Value*)make_symbol("bogus"), (Value*)make_number(2)),
                                make_cons_with((Value*)make_symbol("test"),  (Value*)make_number(4)));

  Value *result = parse_form("test", &error);
  Value *num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 4);

  result = parse_form("bork", &error);
  Value *nope = eval(result, env, &error);
  ASSERT_EQ(NULL, nope);
  ASSERT(error);
  ASSERT_STR_EQ("Couldn't find symbol in scope: bork", error->message);

  PASS();
}

TEST evaluates_set(void) {
  ConsValue *env = make_list(1, make_cons_with((Value*)make_symbol("test"),  (Value*)make_number(4)));

  Error* error = NULL;
  Value* result = parse_form("(set! test 311)", &error);
  Value* num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 311);

  result = parse_form("test", &error);
  Value* new_num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 311);

  result = parse_form("(set! bogus \"oh no\")", &error);
  Value* nope = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  ASSERT_EQ(NULL, nope);

  PASS();
}

TEST evaluates_lambda(void) {
  ConsValue *env = init_global_env();

  Error* error = NULL;
  Value* result = parse_form("((lambda (x) x) 2)", &error);
  Value* num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 2);

  result = parse_form("((lambda (x) ((lambda (y) (+ x y)) 3)) 2)", &error);
  num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

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
