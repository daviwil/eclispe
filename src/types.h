#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char ValueType;

typedef struct {
  ValueType type;
} Value;

typedef struct {
  ValueType type;
  short bool_value;
} BooleanValue;

typedef struct {
  ValueType type;
  short number_value;
} NumberValue;

typedef struct {
  ValueType type;
  float float_value;
} FloatValue;

typedef struct {
  ValueType type;
  char *string_value;
} StringValue;

typedef struct {
  ValueType type;
  char *symbol_name;
} SymbolValue;

typedef struct {
  short type;
} ConsValue;

enum ValueTypes {
  NilValueType = 0,
  BooleanValueType,
  NumberValueType,
  FloatValueType,
  StringValueType,
  SymbolValueType,
  ConsValueType
};

SymbolValue* make_symbol(char *symbol_name);

#endif
