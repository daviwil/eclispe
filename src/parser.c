#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "./types.h"
#include "./parser.h"

#define is_whitespace(c) (c == ' ' || c == '\n' || c == '\t')
#define is_boundary(c) (is_whitespace(c) || c == ')')

// This is a temporary structure for tracking lists being parsed
typedef struct {
  ConsValue* head;
  ConsValue* tail;
} ParsedList;

void push_char(char *buffer, int* buffer_len, char c) {
  (*buffer_len)++;
  buffer[*buffer_len] = c;
  buffer[*buffer_len + 1] = 0;
}

void reset_buffer(char *buffer, int* buffer_len) {
  *buffer_len = -1;
  buffer[0] = 0;
}

void push_cons(ParsedList* cons_stack, unsigned int* cons_depth, ConsValue** current_cons) {
  ConsValue* cons = make_cons();
  cons_stack[*cons_depth].head = cons;
  cons_stack[*cons_depth].tail = cons;

  *current_cons = cons;
  (*cons_depth)++;

  /* printf("Pushed cons!\n"); */
}

ConsValue *pop_cons(ParsedList* cons_stack, unsigned int* cons_depth, ConsValue** current_cons) {
  // TODO: Assert depth > 0
  (*cons_depth)--;
  *current_cons = cons_stack[*cons_depth].tail;

  /* printf("Popped cons!\n"); */

  return cons_stack[*cons_depth].head;
}

ConsValue* push_list(ConsValue* cons, Value *value) {
  /* printf("Pushing value: "); */
  /* print_value(value); */
  /* puts(""); */

  ConsValue* next_cons = make_cons();
  next_cons->car = value;
  cons->cdr = (Value*)next_cons;
  return next_cons;
}

ConsValue* clone_cons(ConsValue* original_cons) {
  ConsValue* new_cons = malloc(sizeof(ConsValue));
  memcpy(new_cons, original_cons, sizeof(ConsValue));
  return new_cons;
}

Value* finish_value(ValueType value_type, char* value_buffer, int* buffer_len) {
  Value* value = NULL;

  switch (value_type) {
    case NumberValueType:
      value = (Value*) make_number(atoi(value_buffer));
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

// Arbitrarily picking the maximum depth of list forms, this will probably need
// to be increased.
const unsigned short MAX_DEPTH = 100;

Value* parse_form(char *form_string) {
  ValueType current_type = 0;
  Value* current_value = NULL;

  unsigned int depth = 0;
  unsigned char is_pair = 0;
  ParsedList cons_stack[MAX_DEPTH];
  ConsValue *current_cons = NULL;

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
        push_cons(cons_stack, &depth, &current_cons);
      } else if (c == ')') {
        // Pop the cons element
        if (depth > 0) {
          /* puts("Finishing cons!"); */
          ConsValue* popped_cons = pop_cons(cons_stack, &depth, &current_cons);
          if (depth > 0) {
            // Push the popped cons as a list item
            current_type = ConsValueType;
            current_cons = push_list(current_cons, (Value*)popped_cons);
          } else {
            // Finish the cons and return it as a value
            current_type = 0;
            current_cons = NULL;
            current_value = (Value*)popped_cons;
          }
        } else {
          // TODO: Parse error
          printf("ERROR: Unmatched close parentheses\n");
        }

        depth--;
      } else if (c == '.' && current_type == ConsValueType) {
        if (is_pair == 0) {
          /* puts("Is pair!"); */
          is_pair = 1;
        } else {
          // TODO: Error
          printf("Can't have more than one '.' in a pair!\n");
          break;
        }
      } else if (isdigit(c)) {
        current_type = NumberValueType;
        push_char(buffer, &buffer_len, c);
      } else if (isalpha(c)) {
        current_type = SymbolValueType;
        push_char(buffer, &buffer_len, c);
      } else if (is_whitespace(c)) {
        /* puts("Whitespace"); */
        // Ignore whitespace at this level
      } else {
        printf("Unexpected character: %c\n", c);
      }
    } else if (current_type == NumberValueType) {
      if (isdigit(c)) {
        push_char(buffer, &buffer_len, c);
      } else if (is_boundary(c)) {
        current_value = finish_value(current_type, buffer, &buffer_len);

        if (depth > 0) {
          current_type = ConsValueType;
          if (current_cons->car == NULL) {
            current_cons->car = current_value;
          } else if (current_cons->cdr != NULL) {
            printf("ERROR: cdr already set!");
            break;
          } else if (is_pair == 1) {
            current_cons->cdr = current_value;
            is_pair = 0;
          } else {
            // Add to the list
            /* puts("Pushing number!"); */
            current_cons = push_list(current_cons, current_value);
          }

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
      if (isalpha(c)) {
        push_char(buffer, &buffer_len, c);
      } else if (is_boundary(c)) {
        current_value = finish_value(current_type, buffer, &buffer_len);

        if (depth > 0) {
          current_type = ConsValueType;
          if (current_cons->car == NULL) {
            /* puts("Setting car!"); */
            current_cons->car = current_value;
          } else if (current_cons->cdr != NULL) {
            printf("ERROR: cdr already set!");
            break;
          } else if (is_pair == 1) {
            current_cons->cdr = current_value;
            is_pair = 0;
          } else {
            // Add to the list
            /* puts("Pushing symbol!"); */
            current_cons = push_list(current_cons, current_value);
          }

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

    if (depth == 0 && current_cons != NULL) {
      // TODO: Error on incomplete forms
      // TODO: Is this case even possible?  depth would be 1 at least
    } else {
      current_value = finish_value(current_type, buffer, &buffer_len);
    }
  }

  /* printf("Returning value of type: %d\n", current_value->type); */

  return current_value;
}
