#include "uart.h"

extern "C" void kernel_main() {
    uint64_t stack_ptr;
    asm volatile("mv %0, sp" : "=r"(stack_ptr));

    kprintf("=== RISC-V Bare Metal Kernel ===\n");
    kprintf("Current Stack Pointer (SP): 0x%p\n", stack_ptr);
    kprintf("Testing integer: %d, Hex: 0x%x, String: %s\n", -42, 0xDEADBEEF, "OK");

    while (true) {
        asm volatile("nop");
    }
}