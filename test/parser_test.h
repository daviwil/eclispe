#include "./greatest.h"
#include "../src/parser.h"

#define check_number(value, expected_value)    \
  ASSERT_EQ(NumberValueType, value->type);     \
  ASSERT(((NumberValue*)value)->number_value); \

#define check_symbol(value, expected_value)   \
  ASSERT_EQ(SymbolValueType, value->type);    \
  ASSERT(((SymbolValue*)value)->symbol_name); \

#define check_car(cons_var, expected_type, car_var)            \
  ASSERT_EQ(ConsValueType, cons_var->type);                    \
  ASSERT(((ConsValue*)cons_var)->car);                         \
  ASSERT_EQ(expected_type, ((ConsValue*)cons_var)->car->type); \
  car_var = ((ConsValue*)cons_var)->car;

#define check_cdr(cons_var, expected_type, cdr_var)            \
  ASSERT_EQ(ConsValueType, cons_var->type);                    \
  ASSERT(((ConsValue*)cons_var)->cdr);                         \
  ASSERT_EQ(expected_type, ((ConsValue*)cons_var)->cdr->type); \
  cdr_var = ((ConsValue*)cons_var)->cdr;

TEST form_is_number(char* input, int expected) {
  Value* result = parse_form(input);
  ASSERT_EQ(NumberValueType, result->type);
  ASSERT_EQ(expected, ((NumberValue*)result)->number_value);

  PASS();
}

TEST parser_parses_numbers(void) {
  Value* result = parse_form("42");
  check_number(result, 42);

  PASS();
}

TEST parser_parses_symbols(void) {
  Value* result = parse_form("symbol");
  check_symbol(result, "symbol");

  PASS();
}

TEST parser_parses_pairs(void) {
  Value* result = parse_form("(test . 1)");
  ASSERT_EQ(ConsValueType, result->type);

  ConsValue* cons = (ConsValue *)result;
  ASSERT(cons->car);
  ASSERT_EQ(SymbolValueType, cons->car->type);
  ASSERT_STR_EQ("test", ((SymbolValue*)cons->car)->symbol_name);

  ASSERT(cons->cdr);
  ASSERT_EQ(NumberValueType, cons->cdr->type);
  ASSERT_EQ(1, ((NumberValue*)cons->cdr)->number_value);

  PASS();
}

TEST parser_parses_lists(void) {
  Value* cons = parse_form("(test 1 2)");

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
  ConsValue* result = (ConsValue *)parse_form("((lambda (x) (+ 1 x)) 2)");
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
  Value *result = parse_form("42   ");
  check_number(result, 42);

  Value *result2 = parse_form("42\n");
  check_number(result2, 42);

  PASS();
}

SUITE(parser_suite) {
    RUN_TEST(parser_parses_numbers);
    RUN_TEST(parser_parses_symbols);
    RUN_TEST(parser_parses_pairs);
    RUN_TEST(parser_parses_lists);
    RUN_TEST(parser_parses_nested_lists);

    RUN_TEST(parser_ends_at_boundaries);
}
