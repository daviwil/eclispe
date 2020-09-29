#include <stdio.h>
#include <stdlib.h>

#include "./log.h"

long MEM_TOTAL = 0;
long MEM_FREED = 0;
long MEM_ALLOCATED = 0;

typedef struct {
  size_t size;
  unsigned short ref_count;
} AllocationHeader;

void* mem_alloc(size_t size) {
  size_t full_size = sizeof(AllocationHeader) + size;
  AllocationHeader* allocation = malloc(full_size);
  allocation->size = full_size;

  MEM_TOTAL += full_size;
  MEM_ALLOCATED += full_size;

  log_verbose("Allocated %d bytes at %ld, body at %ld", full_size, allocation, allocation + 1);

  return allocation + 1;
}

void mem_free(void* ptr) {
  AllocationHeader* allocation = ((AllocationHeader*)ptr) - 1;

  MEM_ALLOCATED -= allocation->size;
  MEM_FREED += allocation->size;

  // TODO: Don't free if ref_count > 0

  log_verbose("Freeing %d bytes at %ld [original: %ld]", allocation->size, allocation, ptr);

  free(allocation);
}

long mem_amount_allocated() {
  return MEM_ALLOCATED;
}

void mem_print_status() {
  printf("Memory Allocated: %ld bytes / Memory Freed: %ld bytes / Lifetime Total: %ld bytes\n", MEM_ALLOCATED, MEM_FREED, MEM_TOTAL);
}
