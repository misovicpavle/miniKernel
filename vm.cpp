#include "include/vm.h"
#include "include/page_alloc.h"
// KERNEL SPACE - VIRTUAL MEMORY
#define RAM_END 0x88000000ULL // Example: 128MB DRAM boundary
//initilize the virtual memory for the kernel itself

PageTable kernelPageTable;

extern "C" {
    extern char _text_end[];
    extern char _end[];
}

void kvminit() {
    uint64_t ktext_flags = static_cast<uint64_t>(PTEFlags::Read) | static_cast<uint64_t>(PTEFlags::Execute);
    uint64_t kdata_flags = static_cast<uint64_t>(PTEFlags::Read) | static_cast<uint64_t>(PTEFlags::Write);
    
    uintptr_t text_end_addr = reinterpret_cast<uintptr_t>(_text_end);

    // 1. Map Kernel Code (.text): 0x80000000 to _text_end (Read + Execute)
    for (uintptr_t address = 0x80000000ULL; address < text_end_addr; address += PAGE_SIZE) {
        map_page(kernelPageTable, address, address, ktext_flags);
    }

    // 2. Map Kernel Data / RAM: _text_end to RAM_END (Read + Write)
    for (uintptr_t address = text_end_addr; address < RAM_END; address += PAGE_SIZE) {
        map_page(kernelPageTable, address, address, kdata_flags);
    }
}

bool map_page(PageTable& processTable, uintptr_t virtual_address, uintptr_t physical_address, uint64_t Flags) {
    // 1. RISC-V Safety Check: W=1, R=0 is illegal
    bool is_write = Flags & static_cast<uint64_t>(PTEFlags::Write);
    bool is_read  = Flags & static_cast<uint64_t>(PTEFlags::Read);
    if (is_write && !is_read) return false;

    // 2. Traverse tree to get pointer to Level 0 PTE
    PageTableEntry* pte = walk(processTable, virtual_address, true);
    if (pte == nullptr) return false; // Out of memory

    // 3. Check if address is already mapped
    auto valid_bit = static_cast<uint64_t>(PTEFlags::Valid);
    if (pte->PTE & valid_bit) return false; 

    // 4. Construct entry and write through the Level 0 pointer
    uint64_t PPN = physical_address >> 12;
    pte->PTE = (PPN << 10) | Flags | valid_bit;

    return true;
}



PageTableEntry* walk(PageTable& rootTable, uintptr_t virtual_address, bool alloc) {
    // PageTableEntry (PTE) is actually a pointer (instaed of a valid entry)
    // to a lower page table level
    PageTable* currentTable = &rootTable;
    auto tree_level = 2;
    while (tree_level > 0) { //slot is a ptr 
        auto VPN = extract_vpn(virtual_address, tree_level);
        auto PTE = currentTable->PTE[VPN];
        if (!(PTE.PTE & static_cast<uint64_t>(PTEFlags::Valid))) {
            if (alloc) {
                auto address = alloc_page();
                //memset(address)
                auto physical_address = reinterpret_cast<uintptr_t>(address);
                auto newPPN = physical_address >> 12;
                auto valid_bit = static_cast<uint64_t>(PTEFlags::Valid);
                auto newPTE = (newPPN << 10) | valid_bit;
                currentTable->PTE[VPN].PTE = newPTE; //place new allocated entry
            }
            else { return nullptr; }
        }
        currentTable = reinterpret_cast<PageTable*>((currentTable->PTE[VPN].PTE >> 10) << 12); //mode down to lower page table
        tree_level--;
    }
    auto VPN0 = extract_vpn(virtual_address, 0);
    return &currentTable->PTE[VPN0];
}