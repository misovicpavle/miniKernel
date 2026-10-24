#include "include/vm.h"
#include "include/page_alloc.h"
// KERNEL SPACE - VIRTUAL MEMORY
//initilize the virtual memory for the kernel itself

PageTable kernelPageTable;
PageTable kerbekPageTableMB;
PageTable kerbekPageTableKB;

void kvminit() {
    //kernel .text section - Read + Exec
    uint64_t ktext_flags = static_cast<uint64_t>(PTEFlags::Read) | static_cast<uint64_t>(PTEFlags::Execute) ;
    uint64_t text_end = text_end; //LINKER FLAG
    for (uintptr_t address = 0x80000000; address < text_end; address += PAGE_SIZE) {
        map_page(kernelPageTable, address, address, ktext_flags); //VA & PA are the same for the kernel
    }
}

//this only supports virtual addresses with Read/Write Flags AKA only 4KB mappings
bool map_page(PageTable processTable, uintptr_t virtual_address, uintptr_t physical_address, uint64_t Flags) {
    /*
    SAFETY RISC-V CHECKS: 
    1. W=1, R=0 - RISC-V does NOT support write-only pages:
    2. R=0,W=0,X=0(this normally is fine and means its a pointer 
                   to lower-page-tree level, in kernel theres none so its illegal)
    3. if walk() is false, no more memory, illegal mapping
    4. if V=1, someone is already using it, return false, illegal mapping
    */    
    if ((Flags & static_cast<uint64_t>(PTEFlags::Write) & !(Flags & static_cast<uint64_t>(PTEFlags::Read)))) return false; //1.
    if (Flags & static_cast<uint64_t>(PTEFlags::Valid)) return false; //4.
    if (!(Flags & static_cast<uint64_t>(PTEFlags::Write) & !(Flags & static_cast<uint64_t>(PTEFlags::Read) & !(Flags & static_cast<uint64_t>(PTEFlags::Execute))))) { 
        walk();
    }
    //Make an kernel page table entry:
    uint64_t VPN = extract_vpn(virtual_address, 0); //tree level 0, 4KB
    PageTableEntry newEntry; //make a new mapping entry
    //PTE = |12b offset(empty)|PPN-44b|RSW 2b|D|A|G|U|X|W|R|V flag-1b
    auto PPN = (physical_address >> 12); //PPN - 2^12=4096(PAGE_SIZE) 
    auto user_flags = static_cast<uint64_t>(Flags);
    auto valid_bit = static_cast<uint64_t>(PTEFlags::Valid); 
    newEntry.PTE = PPN << 10 | user_flags | valid_bit;
    //when shifting binary <<, hardware fills with 0 automatically by how much you <<
    
    kernelPageTable.PTE[VPN] = newEntry;

    return true;
}

bool walk() {
    // PageTableEntry (PTE) is actually a pointer (instaed of a valid entry)
    // to a lower page table level
    kernelPageTable.PTE

}
