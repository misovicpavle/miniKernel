#pragma once

#include <stdint.h>
#include <cstdint>
#include <stddef.h>

enum class IPI_TYPE { 
    TLB_FLUSH_ADDR, 
    TLB_FLUSH_ALL, 
    SHUTDOWN,
    YIELD,
    ICACHE_FLUSH,
    FUNC_EXEC, 
}; 

struct IPI_ENTRY {
    IPI_TYPE ipi_instr;
    uint64_t virtual_mem_addr;
};

bool add_ipi_instr(uint64_t target_hart_id, IPI_ENTRY NEW_ENTRY) {} //append right
bool pop_ipi_instr(IPI_ENTRY* removed_entry) {} //pop left
