#include <stdio.h>
#include <stdarg.h>

#include "./types.h"
#include "./list.h"

ConsValue* push_list_item(ConsList* cons_list, Value *value) {
  /* printf("Pushing value: "); */
  /* print_value(value); */
  /* puts(""); */

  // Create the new cons with the specified value
  ConsValue* next_cons = make_cons();
  next_cons->car = value;

  if (cons_list->head == NULL) {
    // This is the first cons, set as head and tail
    cons_list->head = next_cons;
    cons_list->tail = next_cons;
  } else {
    // Update the existing tail with new cons as cdr, set new tail
    cons_list->tail->cdr = (Value*)next_cons;
    cons_list->tail = next_cons;
  }

  return next_cons;
}

ConsValue* make_list(unsigned int size, ...) {
  va_list argp;
  va_start(argp, size);

  ConsList list;
  list.head = NULL;
  list.tail = NULL;

  for (int i = 0; i < size; i++) {
    Value* item = va_arg(argp, Value*);
    if (item != NULL) {
      push_list_item(&list, item);
    }
  }

  va_end(argp);

  return list.head;
}
