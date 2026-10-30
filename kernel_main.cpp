#include "include/uart.h"

extern "C" void kernel_main() {
    uint64_t stack_ptr;
    asm volatile("mv %0, sp" : "=r"(stack_ptr));
    uart_init();
    enable_paging();
    uart_puts("Hi kernel! Virtual memory finally active!\n");
    
    while (true) {
        asm volatile("nop");
    }
}