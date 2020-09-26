#ifndef _LIST_H
#define _LIST_H

#include "./types.h"

// Not an actual value type, mainly a helper intended
// to be used as a value on the stack
typedef struct {
  ConsValue* head;
  ConsValue* tail;
} ConsList;

ConsValue* push_list_item(ConsList* cons_list, Value *value);
ConsValue* make_list(unsigned int size, ...);

#endif
