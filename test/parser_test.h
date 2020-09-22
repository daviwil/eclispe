#include "./greatest.h"
#include "../src/parser.h"

TEST form_is_number(char* input, int expected) {
  Value* result = parse_form(input);
  ASSERT_EQ(NumberValueType, result->type);
  ASSERT_EQ(expected, ((NumberValue*)result)->number_value);

  PASS();
}

TEST form_is_symbol(char* input, char* expected) {
  Value* result = parse_form(input);
  ASSERT_EQ(SymbolValueType, result->type);
  ASSERT_STR_EQ(expected, ((SymbolValue*)result)->symbol_name);

  PASS();
}

TEST parser_parses_numbers(void) {
  CHECK_CALL(form_is_number("42", 42));

  PASS();
}

TEST parser_parses_symbols(void) {
  CHECK_CALL(form_is_symbol("symbol", "symbol"));

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
  Value* result = parse_form("(test 1 2)");
  /* print_value(result); */
  /* puts(""); */

  ASSERT_EQ(ConsValueType, result->type);

  ConsValue* cons = (ConsValue *)result;
  ASSERT(cons->car);
  ASSERT_EQ(SymbolValueType, cons->car->type);
  ASSERT_STR_EQ("test", ((SymbolValue*)cons->car)->symbol_name);
  ASSERT(cons->cdr);

  cons = (ConsValue *)cons->cdr;
  ASSERT(cons->car);
  ASSERT_EQ(NumberValueType, cons->car->type);
  ASSERT_EQ(1, ((NumberValue*)cons->car)->number_value);
  ASSERT(cons->cdr);

  cons = (ConsValue *)cons->cdr;
  ASSERT(cons->car);
  ASSERT_EQ(NumberValueType, cons->car->type);
  ASSERT_EQ(2, ((NumberValue*)cons->car)->number_value);

  PASS();
}

TEST parser_ends_at_boundaries(void) {
  CHECK_CALL(form_is_number("42   ", 42));
  CHECK_CALL(form_is_number("42\n", 42));

  PASS();
}

SUITE(parser_suite) {
    RUN_TEST(parser_parses_numbers);
    RUN_TEST(parser_parses_symbols);
    RUN_TEST(parser_parses_pairs);
    RUN_TEST(parser_parses_lists);

    RUN_TEST(parser_ends_at_boundaries);
}
