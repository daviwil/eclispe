#ifndef _CHECKS_H
#define _CHECKS_H

#define check_number(value, expected_value)                        \
  ASSERT_EQ(NumberValueType, value->type);                         \
  ASSERT_EQ(expected_value, ((NumberValue*)value)->number_value);  \

#define check_string(value, expected_value)                            \
  ASSERT_EQ(StringValueType, value->type);                             \
  ASSERT_STR_EQ(expected_value, ((StringValue*)value)->string_value);  \

#define check_symbol(value, expected_value)                          \
  ASSERT_EQ(SymbolValueType, value->type);                           \
  ASSERT_STR_EQ(expected_value, ((SymbolValue*)value)->symbol_name); \

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

#define check_mem_start() long _mem_allocated = mem_amount_allocated();

#define check_mem_end()                                             \
  long _mem_leaked = mem_amount_allocated() - _mem_allocated;       \
  ASSERT_EQ_FMT(0, _mem_leaked, "Remaining allocation: %ld bytes");

#endif
