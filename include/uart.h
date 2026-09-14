#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdarg.h> //va_list, va_start, va_arg, va_end

void uart_putc(char c);
void uart_puts(const char* str);
void kprintf(const char* format);

#endif