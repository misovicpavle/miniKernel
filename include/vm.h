#ifndef VM_H
#define VM_H
#include "page_alloc.h"

constexpr size_t PTE_SIZE = PAGE_SIZE / sizeof(uint64_t); // 512 entries

#include <stdint.h>
#include <cstdint>
#include <stddef.h>

enum class PTEFlags : uint64_t { 
    Valid = 1ULL << 0,
    Read = 1ULL << 1, 
    Write = 1ULL << 2,
    Execute = 1ULL << 3,
    User = 1ULL << 4,
    Global = 1ULL << 5,
    Accessed = 1ULL << 6,
    Dirty = 1ULL << 7 
};

struct PageTableEntry {
    uint64_t PTE;
};

struct PageTable {
    PageTableEntry PTE[PTE_SIZE]; //root page table - Gigapage(1GB)
};

static inline uint64_t extract_vpn(uintptr_t virtual_address, size_t tree_level) {
    return (virtual_address >> (12 + (tree_level * 9))) & 0x1FF;
}

void kvminit() {}
bool map_page(PageTable& processTable, uintptr_t virtual_address, uintptr_t physical_address, uint64_t Flags) {}
PageTableEntry* walk(PageTable& rootTable, uintptr_t virtual_address, bool alloc) {}
void enable_paging() {}

#endif //VM_H