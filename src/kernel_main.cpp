#include "include/uart.h"
#include <vm.h>

extern "C" void kernel_main() {
    uint64_t stack_ptr;
    asm volatile("mv %0, sp" : "=r"(stack_ptr));
    //uart_init();
    uart_puts("Hi kernel! Your output stream is active!\n");
    
    //trap_init();
    uart_puts("Hi kernel! Your trap handling is active!\n");

    enable_paging();
    uart_puts("Hi kernel! Your virtual memory sv39 paging is active!\n");
    while (true) {
        asm volatile("nop");
    }
}