//
// Created by misovicpavle on 10/5/26.
//

#ifndef COMPILER_FRAME_ALLOCATOR_H
#define COMPILER_FRAME_ALLOCATOR_H
#include <cstdint>

//4KB page frame
struct PageNode {
    PageNode* nextPage; //64bit
    int64_t pageMemory[511];
};

inline PageNode* nextFreePage = nullptr;

void init_frame_allocator() {
    uintptr_t start_addr = 0x80001000;
    uintptr_t end_addr   = 0x88000000;

    PageNode* p = reinterpret_cast<PageNode*>(start_addr);
    nextFreePage = p;

    while (reinterpret_cast<uintptr_t>(p) < end_addr) {
        PageNode* next_p = p + 1; // Advanced by sizeof(PageNode) = 4096 bytes

        if (reinterpret_cast<uintptr_t>(next_p) >= end_addr) {
            p->nextPage = nullptr; // Last page in physical RAM
        } else {
            p->nextPage = next_p;  // Link to next 4KB page frame
        }

        p = next_p;
    }
}

//smart-pointers?
int64_t* alloc_page() {
    //RAM-0x80000000, 4KB-0x1000
    //PageNode p; -> cpu stack variable, gets deleted at return of this function
    if (nextFreePage == nullptr) { //no more memory?
        return nullptr; //failed alloc
    }
}

void* free_page() {

}

#endif //COMPILER_FRAME_ALLOCATOR_H
