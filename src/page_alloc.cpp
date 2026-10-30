#include "page_alloc.h"

typedef struct _node_p {
    _node_p* nextFrame;
} page_node;

//FREE_PAGE_HEAD (PAGE X), PAGE X-1 ... PAGE 0
static page_node* free_page_head = nullptr; 

//expose the end from the linker file:
extern "C" char _end[];
uintptr_t free_ram_start = reinterpret_cast<uintptr_t>(_end);
uintptr_t ram_end = RAM_END; //128MB DRAM Limit on QEMU

//called @ boot-time
void init_page(uintptr_t _start, uintptr_t _end) {
    //DRAM size - chop it into 4KB page frames
    //_start - Start of RAM Memory after kernel & kernel ELF binary
    _start = (_start + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1); //rounding up
    for (uintptr_t addr = _start; addr < _end; addr += PAGE_SIZE) {
        page_node* new_page = (page_node *) addr;
        new_page->nextFrame = free_page_head;
        free_page_head = new_page; 
        // [ FPH ] -> NEW_PAGE -> REST OF THE LIST...
    }

}

void* alloc_page() {
    //whats the overhead of auto? should i use it?
    page_node* selectedPage = free_page_head;

    if (free_page_head == nullptr) return nullptr; //OUT OF RAM

    free_page_head = free_page_head->nextFrame;
    selectedPage->nextFrame = nullptr; //dc from list
    return (void *)selectedPage;
}

void free_page(void* page) {
    if (page == nullptr) return;
    page_node* newPage = (page_node *) page;
    newPage->nextFrame = free_page_head;
    free_page_head = newPage;
}
