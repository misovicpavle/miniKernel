#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

typedef struct __node_h {
    size_t magic;
    uint16_t size;
} node_h;

typedef struct __node_t {
    struct __node_h* header;
    struct __node_t* next;
} node_t;

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif // HEAP_H