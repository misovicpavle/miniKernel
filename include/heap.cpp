#include "heap.h"
#include <cstddef> //for size_t
#include <cstdint> //for uint16_t

#define NULL __null
#define SIZE_MAX 0x7FFFFFFF
/*
A simple heap implementation for the miniKernel.
THIS IS PER-PROCESS CODE
*/

static node_t* free_list_head = nullptr;
static node_t* free_list_tail = nullptr;

size_t min(size_t& x, size_t& y) {
    if (x >= y) return y;
    return x; 
}

void* kmalloc(size_t size) {
    //check free list bin
      //we need a free-list (bin) now:
    if (size == 0) return nullptr;

    //first allocation?
    if (free_list_head == nullptr) {
        node_t* new_alloc = (node_t *)alloc_page(); //return from syscall)
        //check address space - prevent buffer overflows
        ASSERT(magic == (new_alloc->header->magic));
        new_alloc->header->size = size;
        free_list_tail->next = new_alloc;
        free_list_tail->header->size = new_alloc->header->size;
        //because its the first node in the list:
        free_list_head->header->size = size;
        free_list_head->next = new_alloc;
    }
    //new allocation call after syscall
    else {
        node_t* current_node = free_list_head;
        int offset = -1;
        size_t smallest_diff = SIZE_MAX;

        //FIND EXACT SIZE NODE OR CLOSEST SIZED NODE:
        while (current_node) {
            if (current_node->header->size == size) {
                //pop it from the list 
                return (void *)((char *)current_node + sizeof(node_h));
            }
            else if (current_node->header->size > size) {
                //find the closest sized node for the request size
                int current_node_size = current_node->header->size - size;
                size_t size_difference = current_node->header->size - size;
                smallest_diff = min(smallest_diff, size_difference);
            }
            current_node = current_node->next; //traverses
        }
        //we didnt find an exact match, lets take the closest find:
        current_node = free_list_head;
        if (offset == -1) {
            offset = 0;
            while (current_node) {
                if((current_node->header->size - size) != smallest_diff) {
                    offset++;
                }
                else {
                    offset++; //increment here one more time because otherwise it points to the not equal, previous node of the target node
                    break;
                }
                current_node = current_node->next;
                //find which node is the minimal difference in sizes, and increment offset by 1 for every failed lookup
            }
        }

        //SLICE THE NODE TO THE SIZE DESIRED:
        //REWIRE THE FREE LIST:
        else {
            node_t* target_node = free_list_head + offset; //this finds our target node efficiently, no loop needed
            int new_size = target_node->header->size - size;
            node_t* new_remainder_node;
            (free_list_head + offset)->header->size = new_size; //update old node to a new size
            node_t* return_node;
            return_node->header->magic = target_node->header->magic;
            return_node->header->size = size;
            return_node->next = nullptr;
            return (void *)((char *)return_node + sizeof(node_h));
        }
    }
    //if none of that works, syscall mmap()

    return;
}

void kfree(void* ptr) {
    node_h* hptr = (node_h*) ptr - 1; //get the header node information for giving pointer
    //This ptr is the variable containing only memory aka space, no node_h header info

    //hptr->magic
    size_t size = sizeof(hptr) + hptr->size;
    //syscall returns "size' back to OS via some syscall()
}
