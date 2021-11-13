#include "mem.h"

#include <stdlib.h>
#include <string.h>

static unsigned long memcount = 0;
static unsigned long max_memcount = 0;

static size_t mem_limit = 0;

void* mem__calloc(size_t nmemb, size_t size) {
  void* mem;
  mem = calloc(nmemb, size);

  return mem;
}

void* mem__malloc(size_t size) {
  void* mem;

  mem = malloc(size);

  return mem;
}

void* mem__realloc(void* ptr, size_t size) {
  void* mem;
  mem = realloc(ptr, size);

  return mem;
}

void mem__free(void* mem) { free(mem); }

char* mem__strdup(const char* s) {
  char* str;
  str = strdup(s);

  return str;
}

unsigned long mem__memory_used(void) { return memcount; }

unsigned long mem__max_memory_used(void) { return max_memcount; }

void mem__set_limit(size_t lim) { mem_limit = lim; }
