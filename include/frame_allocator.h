//
// Created by misovicpavle on 10/5/26.
//

#ifndef COMPILER_FRAME_ALLOCATOR_H
#define COMPILER_FRAME_ALLOCATOR_H
#include <cstdint>
/*
DS Approach for paging
| Pointer to next free page - 8 bytes | Rest of the 4KB-8B page to be used |
*/
//4KB page frame
struct PageNode {
    PageNode* nextPage; //64bit
    uintptr_t pageMemory[511];
};

inline PageNode* nextFreePage = nullptr;

void init_frame_allocator() {
    uintptr_t start_addr = 0x80001000; //start of QEMU RAM
    uintptr_t end_addr   = 0x88000000; //end of QEMU RAM

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
//void* instead of uintptr_t* because void can be converted to any type w/o explicit conversions/casting
void* alloc_page() {
    //RAM-0x80000000, 4KB-0x1000
    //PageNode p; -> cpu stack variable, gets deleted at return of this function
    if (nextFreePage == nullptr) { //no more memory?
        return nullptr; //failed alloc
    }
    auto* oldPage = reinterpret_cast<uintptr_t*>(nextFreePage);
    nextFreePage = nextFreePage->nextPage; //move the ptr to the next page, marking the previous as occupied
    return oldPage; //return the point to the new page

}

void* free_page(void* ptr) {
    if (ptr != nullptr) {
        auto* node = reinterpret_cast<PageNode*>(ptr); //move 4KB to the left of the last used page
        node->nextPage = nextFreePage; //give the old page that you're freeing now where it should look for a new page - if someone reuses it
        nextFreePage = node; //this page is now free so update the ptr
    }
}

#endif //COMPILER_FRAME_ALLOCATOR_H
