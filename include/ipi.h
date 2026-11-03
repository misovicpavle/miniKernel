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

void add_ipi_instr() {} //append right
void pop_ipi_instr() {} //pop left
