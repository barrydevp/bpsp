#ifndef _MEM_H_
#define _MEM_H_

#include <stdio.h>
#include <sys/types.h>

void* mem__calloc(size_t nmemb, size_t size);
void* mem__malloc(size_t size);
void* mem__realloc(void* ptr, size_t size);

void mem__free(void* mem);

void mem__memmove(void* dst, const void* src, size_t num);
void mem__memcpy(void* dst, const void* src, size_t num);

char* mem__strdup(const char* s);

unsigned long mem__memory_used(void);
unsigned long mem__max_memory_used(void);

void mem__set_limit(size_t lim);

#endif  // _MEM_H_
