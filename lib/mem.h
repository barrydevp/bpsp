#ifndef _MEM_H_
#define _MEM_H_

#include <stdio.h>
#include <sys/types.h>

void* mem__calloc(size_t nmemb, size_t size);
void* mem__malloc(size_t size);
void* mem__realloc(void* ptr, size_t size);

void mem__free(void* mem);

char* mem_strdup(const char* s);

unsigned long mem_memory_used(void);
unsigned long mem_max_memory_used(void);

void mem_set_limit(size_t lim);

#endif  // _MEM_H_
