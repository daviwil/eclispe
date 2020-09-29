#ifndef _MEM_H
#define _MEM_H

#include <stdio.h>

void* mem_alloc(size_t size);
void mem_free(void* ptr);
long mem_amount_allocated();
void mem_print_status();

#endif
