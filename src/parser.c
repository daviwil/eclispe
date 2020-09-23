#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "./types.h"
#include "./parser.h"

#define is_whitespace(c) (c == ' ' || c == '\n' || c == '\t')
#define is_boundary(c) (is_whitespace(c) || c == ')')
#define is_symbol_char(c) (isalpha(c) || c == '+' || c == '-' || c == '/' || c == '*')

// Arbitrarily picking the maximum depth of list forms, this will probably need
// to be increased.
#define MAX_DEPTH 1000

typedef struct {
  ConsValue* head;
  ConsValue* tail;
} ConsList;

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

  /* printf("Pushed cons!\n"); */
}

ConsValue* pop_cons(ConsStack* cons_stack) {
  /* printf("Popped cons at %d!\n", cons_stack->depth); */

  // TODO: Assert depth > 0
  return cons_stack->stack[--cons_stack->depth].head;
}

// Pushes a list item onto the current cons stack level
ConsValue* push_list_item(ConsStack* cons_stack, Value *value) {
  /* printf("Pushing value: "); */
  /* print_value(value); */
  /* puts(""); */

  // Create the new cons with the specified value
  ConsValue* next_cons = make_cons();
  next_cons->car = value;

  // Update the existing tail with new cons as cdr, set new tail
  cons_stack->stack[cons_stack->depth - 1].tail->cdr = (Value*)next_cons;
  cons_stack->stack[cons_stack->depth - 1].tail = next_cons;

  return next_cons;
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
    push_list_item(cons_stack, value);
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

Value* parse_form(char *form_string) {
  ValueType current_type = 0;
  Value* current_value = NULL;

  unsigned char is_pair = 0;
  ConsStack cons_stack;
  cons_stack.depth = 0;

  // Used for string parsing
  unsigned char is_char_escaped = 0;

  char buffer[100];
  int buffer_len = -1;

  int i = 0;
  int len = strlen(form_string);
  for (; i < len; i++) {
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
          // TODO: Parse error
          printf("ERROR: Unmatched close parentheses\n");
        }
      } else if (c == '.' && current_type == ConsValueType) {
        if (is_pair == 0) {
          is_pair = 1;
        } else {
          // TODO: Error
          printf("Can't have more than one '.' in a pair!\n");
          break;
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
            i--;
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
            i--;
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
            i--;
          }
        }
      }
    }
  }

  if (i == len && current_value == NULL) {

    /* printf("End of string, type: %d, cons: %d\n", current_type, current_cons); */

    // TODO: How to check for incomplete forms in new model?
    /* if (cons_stack.depth == 0) { */
    /*   // TODO: Error on incomplete forms */
    /*   // TODO: Is this case even possible?  depth would be 1 at least */
    /* } else { */
    /*   current_value = finish_value(current_type, buffer, &buffer_len); */
    /* } */

    current_value = finish_value(current_type, buffer, &buffer_len);
  }

  /* printf("Returning value of type: %d\n", current_value->type); */
  /* print_value(current_value); */
  /* puts(""); */

  return current_value;
}
