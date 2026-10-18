#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H
#define PAGE_SIZE 4096

#include <stddef.h>
#include <stdint.h>

void init_page(uintptr_t _start, uintptr_t _end);
void* alloc_page();
void free_page(void* page);


#endif //PAGE_ALLOC_H