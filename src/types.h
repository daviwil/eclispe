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
  ValueType type;
  Value *car;
  Value *cdr;
} ConsValue;

// Defines the signature for primitive function pointers.
// Accepts an environment as parameter, all arguments will
// be bound within it.
typedef Value* (*PrimFunc)(ConsValue*);

typedef struct {
  ValueType type;
  ConsValue *args;
  ConsValue *body;
  ConsValue *env;
  PrimFunc invoker;
} FunctionValue;

enum ValueTypes {
  NilValueType = 0,
  BooleanValueType,
  NumberValueType,
  FloatValueType,
  StringValueType,
  SymbolValueType,
  ConsValueType,
  FunctionValueType,
};

NumberValue* make_number(int number_value);
SymbolValue* make_symbol(char* symbol_name);
StringValue* make_string(char* string_value);
ConsValue* make_cons();
ConsValue* make_cons_with(Value* car, Value* cdr);
FunctionValue* make_function(ConsValue* body, ConsValue* args, ConsValue* env);
FunctionValue* make_prim_function(PrimFunc invoker, ConsValue* args, ConsValue* env);

void print_value(Value *value);

#endif
