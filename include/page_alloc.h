#ifndef PAGE_ALLOC_H
#define PAGE_ALLOC_H
#define PAGE_SIZE 4096
#define SYS_MEM 128 * 1024 * 1024
#define QEMU_START 0x80000000
#define RAM_END (QEMU_START + SYS_MEM); 
//START OF RAM @ QEMU (0x80000000) + 128MB DRAM Limit on QEMU

#include <stddef.h>
#include <stdint.h>

void init_page(uintptr_t _start, uintptr_t _end);
void* alloc_page();
void free_page(void* page);

#endif //PAGE_ALLOC_H