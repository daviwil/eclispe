#include "./greatest.h"
#include "../src/list.h"
#include "../src/parser.h"
#include "../src/eval.h"
#include "./checks.h"

TEST evaluates_numbers(void) {
  check_mem_start();

  Error* error = NULL;
  Value *result = parse_form("42", &error);
  ASSERT_EQ(NULL, error);

  Value *num = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 42);

  // TODO: This exposes a problem - when eval returns a value directly
  // as a result of a computation, we either can't free the original
  // value or we need to increase its refcount so that the returned
  // value can be freed separately at a later time.  However, it's
  // not easy to know exactly when the value has been used unless we
  // refcount every time the value is used in a binding; this will
  // require fastidious checks...
  free_value(result);
  check_mem_end();

  PASS();
}

TEST evaluates_strings(void) {
  check_mem_start();

  Error* error = NULL;
  Value *result = parse_form("\"hello world!\"", &error);
  Value *str = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_string(str, "hello world!");

  free_value(result);
  check_mem_end();

  PASS();
}

TEST evaluates_if(void) {
  check_mem_start();

  Error* error = NULL;
  Value *result = parse_form("(if 1 2 3)", &error);
  Value *num = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 2);

  free_value(result);
  check_mem_end();

  PASS();
}

TEST evaluates_quote(void) {
  check_mem_start();

  Error* error = NULL;
  Value *result = parse_form("(quote hamburger)", &error);
  Value *symbol = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_symbol(symbol, "hamburger");

  free_value(result);
  check_mem_end();

  PASS();
}

TEST evaluates_begin(void) {
  check_mem_start();

  Error* error = NULL;
  Value *result = parse_form("(begin 1 2 3)", &error);
  Value *num = eval(result, NULL, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 3);

  free_value(result);
  check_mem_end();

  PASS();
}

TEST evaluates_symbol(void) {
  check_mem_start();

  Error* error = NULL;
  ConsValue *env = make_list(2, make_cons_with((Value*)make_symbol("bogus"), (Value*)make_number(2)),
                                make_cons_with((Value*)make_symbol("test"),  (Value*)make_number(4)));

  Value *result = parse_form("test", &error);
  Value *num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 4);

  free_value(result);

  result = parse_form("bork", &error);
  Value *nope = eval(result, env, &error);
  ASSERT_EQ(NULL, nope);
  ASSERT(error);
  ASSERT_STR_EQ("Couldn't find symbol in scope: bork", error->message);

  free_error(error);
  free_value(result);
  free_value((Value*)env);
  check_mem_end();

  PASS();
}

TEST evaluates_set(void) {
  check_mem_start();

  ConsValue *env = make_list(1, make_cons_with((Value*)make_symbol("test"),  (Value*)make_number(4)));

  Error* error = NULL;
  Value* result = parse_form("(set! test 311)", &error);
  Value* num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 311);

  /* free_value(result); */

  result = parse_form("test", &error);
  Value* new_num = eval(result, env, &error);
  ASSERT_EQ(NULL, error);

  check_number(num, 311);

  // TODO: This needs ref counting!
  /* free_value(result); */

  result = parse_form("(set! bogus \"oh no\")", &error);
  Value* nope = eval(result, env, &error);
  // TODO: This should error
  ASSERT_EQ(NULL, error);

  ASSERT_EQ(NULL, nope);

  free_value(result);
  free_value((Value*)env);
  /* check_mem_end(); */

  PASS();
}

TEST evaluates_lambda(void) {
  check_mem_start();

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

  free_value(result);
  free_value((Value*)env);

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
