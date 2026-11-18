#include "heap.h"
#include <cstddef> //for size_t
#include <cstdint> //for uint16_t

#define NULL __null
#ifndef SIZE_MAX
#define SIZE_MAX 0x7FFFFFFF
#endif
#define PAGE_SIZE 4096
#define MINSIZE 8 //smallest payload worth splitting off into its own free node
#define HEAP_MAGIC 0xC0FFEE
/*
A simple heap implementation for the miniKernel.
THIS IS PER-PROCESS CODE

Block layout: [node_t][node_h][payload...]
node->header points right after the node, the user gets the address right after the header.
*/

void* alloc_page(); //returns one 4KB page (syscall)

static node_t* free_list_head = nullptr;
static node_t* free_list_tail = nullptr;

size_t min(size_t& x, size_t& y) {
    if (x >= y) return y;
    return x;
}


//THIS IS THE LAYOUR OF THE FULL NODE
//grab a fresh page and turn it into one big free node (not yet linked into the list)
static node_t* new_page() {
    node_t* new_alloc = (node_t *)alloc_page(); //return from syscall
    if (new_alloc == nullptr) return nullptr;
    new_alloc->header = (node_h*)(new_alloc + 1); //header sits right after the node
    new_alloc->header->magic = HEAP_MAGIC;
    new_alloc->header->size = PAGE_SIZE - sizeof(node_t) - sizeof(node_h); //whole page is free
    new_alloc->next = nullptr;
    return new_alloc;
}

//remove node from the free list, prev is the node before it (nullptr if node is the head)
static void unlink(node_t* prev, node_t* node) {
    if (prev) prev->next = node->next;
    else free_list_head = node->next;
    if (node == free_list_tail) free_list_tail = prev;
    node->next = nullptr; //node is the node to removed, also detach where it points to
}

void* kmalloc(size_t size) {
    //check free list bin
      //we need a free-list (bin) now:
    if (size == 0) return nullptr;
    size = (size + 7) & ~(size_t)7; //keep every block 8-byte aligned
    if (size > PAGE_SIZE - sizeof(node_t) - sizeof(node_h)) return nullptr; //won't fit in a page

    //first allocation?
    if (free_list_head == nullptr) {
        node_t* new_alloc = new_page();
        if (new_alloc == nullptr) return nullptr;
        //because its the first node in the list:
        free_list_head = new_alloc;
        free_list_tail = new_alloc;
    }
    //now search the list (also right after the first page was fetched)
    {
        node_t* current_node = free_list_head;
        node_t* prev_node = nullptr;
        size_t smallest_diff = SIZE_MAX;

        //FIND EXACT SIZE NODE OR CLOSEST SIZED NODE:
        while (current_node) {
            if (current_node->header->size == size) {
                //pop it from the list
                unlink(prev_node, current_node);
                return (void *)(current_node->header + 1);
            }
            else if (current_node->header->size > size) {
                //find the closest sized node for the request size
                size_t size_difference = current_node->header->size - size;
                smallest_diff = min(smallest_diff, size_difference);
            }
            prev_node = current_node;
            current_node = current_node->next; //traverses
        }

        //if none of that works, nothing is big enough -> fetch a new page and append it to the tail
        if (smallest_diff == SIZE_MAX) {
            node_t* new_alloc = new_page();
            if (new_alloc == nullptr) return nullptr;
            free_list_tail->next = new_alloc;
            free_list_tail = new_alloc;
            smallest_diff = new_alloc->header->size - size;
        }

        //we didnt find an exact match, lets take the closest find:
        current_node = free_list_head;
        prev_node = nullptr;
        while (current_node) {
            //find which node is the minimal difference in sizes
            if (current_node->header->size >= size && current_node->header->size - size == smallest_diff) {
                break;
            }
            prev_node = current_node;
            current_node = current_node->next;
        }
        node_t* target_node = current_node;

        //SLICE THE NODE TO THE SIZE DESIRED:
        size_t new_size = target_node->header->size - size;
        if (new_size < sizeof(node_t) + sizeof(node_h) + MINSIZE) {
            //leftover too small to be its own node, hand out the whole thing
            unlink(prev_node, target_node);
            return (void *)(target_node->header + 1);
        }
        //remainder starts right after the returned payload
        node_t* new_remainder_node = (node_t*)((char*)(target_node->header + 1) + size);
        new_remainder_node->header = (node_h*)(new_remainder_node + 1);
        new_remainder_node->header->magic = target_node->header->magic;
        new_remainder_node->header->size = new_size - sizeof(node_t) - sizeof(node_h); //update old node to a new size
        new_remainder_node->next = target_node->next;

        //REWIRE THE FREE LIST: remainder takes the target's place
        if (prev_node) prev_node->next = new_remainder_node;
        else free_list_head = new_remainder_node;
        if (target_node == free_list_tail) free_list_tail = new_remainder_node;

        node_t* return_node = target_node;
        return_node->header->size = size;
        return_node->next = nullptr;
        return (void *)(return_node->header + 1);
    }

    return nullptr;
}

void kfree(void* ptr) {
    if (ptr == nullptr) return;
    node_h* hptr = (node_h*) ptr - 1; //get the header node information for giving pointer
    //This ptr is the variable containing only memory aka space, no node_h header info
    node_t* node = (node_t*) hptr - 1; //the node sits right before the header

    //hptr->magic
    size_t size = sizeof(node_h) + hptr->size;
    //syscall returns "size' back to OS via some syscall()

    //for now push the block back onto the free list
    node->next = free_list_head;
    free_list_head = node;
    if (free_list_tail == nullptr) free_list_tail = node;
}
