#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "./types.h"
#include "./log.h"
#include "./error.h"
#include "./list.h"
#include "./parser.h"

#define is_whitespace(c) (c == ' ' || c == '\n' || c == '\t')
#define is_boundary(c) (is_whitespace(c) || c == ')')
#define is_symbol_char(c) (isalpha(c) || c == '+' || c == '-' || c == '/' || c == '*' || c == '!'|| c == '?')

// Arbitrarily picking the maximum depth of list forms, this will probably need
// to be increased.
#define MAX_DEPTH 1000

typedef struct {
  unsigned int depth;
  ConsList stack[MAX_DEPTH];
} ConsStack;

void push_char(char *buffer, int* buffer_len, char c) {
  (*buffer_len)++;
  buffer[*buffer_len] = c;
  buffer[*buffer_len + 1] = 0;
}

void reset_buffer(char *buffer, int* buffer_len) {
  *buffer_len = -1;
  buffer[0] = 0;
}

void push_cons(ConsStack* cons_stack) {
  ConsValue* cons = make_cons();
  cons_stack->depth++;
  cons_stack->stack[cons_stack->depth - 1].head = cons;
  cons_stack->stack[cons_stack->depth - 1].tail = cons;

  log("Pushed cons!");
}

ConsValue* pop_cons(ConsStack* cons_stack) {
  log("Popped cons at %d!", cons_stack->depth);

  // TODO: Assert depth > 0
  return cons_stack->stack[--cons_stack->depth].head;
}

// Pushes a list item onto the current cons stack level
ConsValue* push_stack_list_item(ConsStack* cons_stack, Value *value) {
  return push_list_item(&cons_stack->stack[cons_stack->depth - 1], value);
}

void set_current_cons_value(ConsStack* cons_stack, Value* value, unsigned char *is_pair) {
  // TODO: Assert depth > 0
  ConsValue* current_cons = cons_stack->stack[cons_stack->depth - 1].tail;

  if (current_cons->car == NULL) {
    current_cons->car = value;
  } else if (*is_pair == 1) {
    current_cons->cdr = value;
    *is_pair = 0;
  } else {
    push_stack_list_item(cons_stack, value);
  }
}

Value* finish_value(ValueType value_type, char* value_buffer, int* buffer_len) {
  Value* value = NULL;

  switch (value_type) {
    case NumberValueType:
      value = (Value*) make_number(atoi(value_buffer));
      break;
    case StringValueType:
      value = (Value*) make_string(value_buffer);
      break;
    case SymbolValueType:
      value = (Value*) make_symbol(value_buffer);
      break;
    default:
      // TODO: Assert
      printf("Finishing unexpected value type: %d\n", value_type);
      break;
  }

  reset_buffer(value_buffer, buffer_len);

  return value;
}

void move_cursor(int delta, unsigned int* i, unsigned int* line, unsigned int* col) {
  *i = *i + delta;

  // TODO: How do I know how long each line is?

  if (delta < 0) {
  } else {
  }
}

Value* parse_form(char *form_string, Error** error) {
  log_set_scope("parser");

  ValueType current_type = 0;
  Value* current_value = NULL;

  unsigned int source_line = 1;
  unsigned int source_col = 0;

  unsigned char is_pair = 0;
  ConsStack cons_stack;
  cons_stack.depth = 0;

  // Used for string parsing
  unsigned char is_char_escaped = 0;

  char buffer[1000];
  int buffer_len = -1;

  unsigned int i = 0;
  unsigned int len = strlen(form_string);

  while (i < len) {
    char c = form_string[i];

    /* printf("Type: %d | Character %d: %c\n", current_type, i, c); */

    if (current_type == 0 || current_type == ConsValueType) {
      // What's the next token?
      if (c == '(') {
        // Prepare the cons for this depth
        current_type = ConsValueType;
        push_cons(&cons_stack);
      } else if (c == ')') {
        // Pop the cons element
        if (cons_stack.depth > 0) {
          ConsValue* popped_cons = pop_cons(&cons_stack);
          if (cons_stack.depth > 0) {
            // Push the popped cons as a list item
            current_type = ConsValueType;
            set_current_cons_value(&cons_stack, (Value*)popped_cons, &is_pair);
          } else {
            // Finish the cons and return it as a value
            current_type = 0;
            current_value = (Value*)popped_cons;
          }
        } else {
          // TODO: Add source location
          *error = make_error("Unmatched close parentheses");
          return NULL;
        }
      } else if (c == '.' && current_type == ConsValueType) {
        if (is_pair == 0) {
          is_pair = 1;
        } else {
          // TODO: Add source location
          *error = make_error("Can't have more than one '.' in a pair");
          return NULL;
        }
      } else if (c == '"') {
        // TODO: One day I'll have to deal with Unicode
        // https://www.cprogramming.com/tutorial/unicode.html
        current_type = StringValueType;
      } else if (isdigit(c)) {
        current_type = NumberValueType;
        push_char(buffer, &buffer_len, c);
      } else if (is_symbol_char(c)) {
        current_type = SymbolValueType;
        push_char(buffer, &buffer_len, c);
      } else if (is_whitespace(c)) {
        // Ignore whitespace at this level
      } else {
        printf("Unexpected character: %c\n", c);
      }
    } else if (current_type == StringValueType) {
      if (c == '"' && is_char_escaped == 0) {
        current_value = finish_value(current_type, buffer, &buffer_len);

        if (cons_stack.depth > 0) {
          set_current_cons_value(&cons_stack, current_value, &is_pair);
          current_type = ConsValueType;

          if (c == ')') {
            // Let the outer case handle it
            move_cursor(-1, &i, &source_line, &source_col);
          }
        }
      } else if (c == '\\') {
        is_char_escaped = 1;
      } else {
        is_char_escaped = 0;
        push_char(buffer, &buffer_len, c);
      }
    } else if (current_type == NumberValueType) {
      if (isdigit(c)) {
        push_char(buffer, &buffer_len, c);
      } else if (is_boundary(c)) {
        current_value = finish_value(current_type, buffer, &buffer_len);

        if (cons_stack.depth > 0) {
          set_current_cons_value(&cons_stack, current_value, &is_pair);
          current_type = ConsValueType;

          if (c == ')') {
            // Let the outer case handle it
            move_cursor(-1, &i, &source_line, &source_col);
          }
        } else {
          // TODO: Maybe don't stop yet so we can error if there's another form
          break;
        }
      }
    } else if (current_type == SymbolValueType) {
      if (is_symbol_char(c)) {
        push_char(buffer, &buffer_len, c);
      } else if (is_boundary(c)) {
        current_value = finish_value(current_type, buffer, &buffer_len);

        if (cons_stack.depth > 0) {
          set_current_cons_value(&cons_stack, current_value, &is_pair);
          current_type = ConsValueType;

          if (c == ')') {
            // Let the outer case handle it
            move_cursor(-1, &i, &source_line, &source_col);
          }
        }
      }
    }

    move_cursor(1, &i, &source_line, &source_col);
  }

  if (i == len) {
    if (cons_stack.depth > 0) {
      log("End of string with unmatched parens, depth: %d", cons_stack.depth);
      *error = make_error("Unmatched open parentheses");
      return NULL;
    } else if (current_value == NULL) {
      log("End of string, finishing type: %d", current_type);
      current_value = finish_value(current_type, buffer, &buffer_len);
    }
  }

  log("Returning value of type: %d", current_value->type);

  return current_value;
}
