#include "./greatest.h"
#include "../src/parser.h"
#include "./checks.h"

TEST form_is_number(char* input, int expected) {
  Error* error = NULL;
  Value* result = parse_form(input, &error);
  ASSERT_EQ(NumberValueType, result->type);
  ASSERT_EQ(expected, ((NumberValue*)result)->number_value);

  PASS();
}

TEST parser_parses_numbers(void) {
  Error* error = NULL;
  Value* result = parse_form("42", &error);
  check_number(result, 42);

  PASS();
}

TEST parser_parses_symbols(void) {
  Error* error = NULL;
  Value* result = parse_form("symbol", &error);
  check_symbol(result, "symbol");

  PASS();
}

TEST parser_parses_strings(void) {
  Error* error = NULL;
  Value* result = parse_form("\"hello world!\"", &error);
  check_string(result, "hello world!");

  Value* result2 = parse_form("\"hello \\\"world!\\\"\"", &error);
  check_string(result2, "hello \"world!\"");

  PASS();
}

TEST parser_parses_pairs(void) {
  Error* error = NULL;
  Value* result = parse_form("(test . 1)", &error);
  ASSERT_EQ(ConsValueType, result->type);

  ConsValue* cons = (ConsValue *)result;
  ASSERT(cons->car);
  check_symbol(cons->car, "test");

  ASSERT(cons->cdr);
  check_number(cons->cdr, 1);

  PASS();
}

TEST parser_parses_lists(void) {
  Error* error = NULL;
  Value* cons = parse_form("(test 1 2)", &error);

  ASSERT_EQ(ConsValueType, cons->type);

  Value* test_symbol = NULL;
  check_car(cons, SymbolValueType, test_symbol);
  check_symbol(test_symbol, "test");

  Value *number_1 = NULL;
  check_cdr(cons, ConsValueType, cons);
  check_car(cons, NumberValueType, number_1);
  check_number(number_1, 1);

  Value *number_2 = NULL;
  check_cdr(cons, ConsValueType, cons);
  check_car(cons, NumberValueType, number_2);
  check_number(number_2, 2);

  PASS();
}

TEST parser_parses_nested_lists(void) {
  Error* error = NULL;
  ConsValue* result = (ConsValue *)parse_form("((lambda (x) (+ 1 x)) 2)", &error);
  ASSERT_EQ(ConsValueType, result->type);

  Value* lambda_expr = NULL;
  check_car(result, ConsValueType, lambda_expr);

  Value* lambda_symbol = NULL;
  check_car(lambda_expr, SymbolValueType, lambda_symbol);
  check_symbol(lambda_symbol, "lambda");

  Value* lambda_cdr = NULL;
  check_cdr(lambda_expr, ConsValueType, lambda_cdr);

  Value* arg_list = NULL;
  check_car(lambda_cdr, ConsValueType, arg_list);

  Value* x_symbol = NULL;
  check_car(arg_list, SymbolValueType, x_symbol);
  check_symbol(x_symbol, "x");

  check_cdr(lambda_cdr, ConsValueType, lambda_cdr);

  Value *plus_expr = NULL;
  check_car(lambda_cdr, ConsValueType, plus_expr);

  Value *plus_symbol = NULL;
  check_car(plus_expr, SymbolValueType, plus_symbol);
  check_symbol(plus_symbol, "+");

  Value* number_2_cons = NULL;
  check_cdr(result, ConsValueType, number_2_cons);

  Value* number_2 = NULL;
  check_car(number_2_cons, NumberValueType, number_2);
  check_number(number_2, 2);

  PASS();
}

TEST parser_ends_at_boundaries(void) {
  Error* error = NULL;
  Value *result = parse_form("42   ", &error);
  check_number(result, 42);

  Value *result2 = parse_form("42\n", &error);
  check_number(result2, 42);

  PASS();
}

TEST parser_errors_unmatched_paren(void) {
  Error* error = NULL;
  ConsValue* result = (ConsValue *)parse_form("((+ 1 x)", &error);
  ASSERT_EQ(result, NULL);

  ASSERT(error);
  ASSERT_STR_EQ("Unmatched open parentheses", error->message);

  error = NULL;
  result = (ConsValue *)parse_form("(+ 1 x))", &error);
  ASSERT_EQ(result, NULL);

  ASSERT(error);
  ASSERT_STR_EQ("Unmatched close parentheses", error->message);

  PASS();
}

SUITE(parser_suite) {
    RUN_TEST(parser_parses_numbers);
    RUN_TEST(parser_parses_strings);
    RUN_TEST(parser_parses_symbols);
    RUN_TEST(parser_parses_pairs);
    RUN_TEST(parser_parses_lists);
    RUN_TEST(parser_parses_nested_lists);
    RUN_TEST(parser_ends_at_boundaries);

    RUN_TEST(parser_errors_unmatched_paren);
}
