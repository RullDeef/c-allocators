#ifndef __CALLOCS_H_
#define __CALLOCS_H_

#include <stddef.h>
#include <stdio.h>

#ifdef CALLOCS_REDEFINE
#define malloc(size) callocs_malloc(size)
#define calloc(num, size) callocs_calloc(num, size)
#define realloc(ptr, size) callocs_realloc(ptr, size)
#define free(ptr) callocs_free(ptr)
#endif

int callocs_init(size_t pool_size);
void callocs_destroy(void);

void *callocs_malloc(size_t size);
void *callocs_calloc(size_t num, size_t size);
void *callocs_realloc(void *ptr, size_t size);
void callocs_free(void *ptr);

size_t callocs_get_max_free_space(void);

int callocs_fprintf_heap_info(FILE *file);

#endif // __CALLOCS_H_
