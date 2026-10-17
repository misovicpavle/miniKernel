#include "include/uart.h"
#include <stdarg.h>
// Bazna MMIO adresa za NS16550A UART kontroler na QEMU virt platformi
volatile uint8_t* const UART_BASE = (volatile uint8_t*)0x10000000;
// Transmitter Holding Register (Offset 0x00): Registar u koji se upisuje bajt (karakter) koji treba poslati
volatile uint8_t* const THR       = UART_BASE + 0x00;
// Line Status Register (Offset 0x05): Statusni registar; Bit 5 (0x20) označava da je THR prazan i spreman za slanje
volatile uint8_t* const LSR       = UART_BASE + 0x05;

//converting raw 64b to ascii
void to_hex(uint64_t number) { //caller prints the "0x" prefix itself
    char buffer[16];
    int i = 0;
    if (number == 0) {
        uart_putc('0');
        return;
    }
    while (number > 0) {
        auto bits = number & 0x000F;
        if (bits > 9) { buffer[i] = (bits - 10) + 'A'; }
        else { buffer[i] = bits + '0'; }
        ++i;
        number >>= 4; //move to the next 4 bits
    }
    while (i > 0) { uart_putc(buffer[--i]); }
}


//decimal conversion
void to_int(int number) {
    //work in unsigned so -INT_MIN doesn't overflow
    uint64_t value = (uint64_t)number;
    if (number == 0) {
        uart_putc('0');
        return;
    }
    else if (number < 0) {
        uart_putc('-');
        value = 0 - (uint64_t)(int64_t)number;
    }

    char buffer[32];
    int i = 0;
    while (value > 0) {
        auto remainder = value % 10;
        buffer[i++] = remainder + '0'; // convert standard digit to ascii via + '0'
        value /= 10;
    }

    while (i > 0) { //write out the number
        uart_putc(buffer[--i]);
    }
}

void kprintf(const char* format, ...) { //... - makes the compiler know that arguments are being passed with registers a1 to a7 from the stack
    va_list args;
    va_start(args, format);

    while (*format != '\0') {
        //% checker
        if (*format == '%') {
            ++format;
            if (*format == '\0') { break; } //lone % at the end of the string
            if (*format == 'c') {
                auto result = va_arg(args, int);
                uart_putc(result);
            }
            else if (*format == 's') {
                const char* result = va_arg(args, const char*);
                uart_puts(result);
            }
            else if (*format == 'd') {
                auto number = va_arg(args, int);
                to_int(number);
            }
            else if (*format == 'x') {
                //%x takes an unsigned int - reading 64 bits would pick up RV64's sign extension
                auto result = va_arg(args, unsigned int);
                to_hex(result);
            }
            else if (*format == 'p') {
                auto result = va_arg(args, uint64_t);
                to_hex(result);
            }
            else if (*format == '%') {
                uart_putc('%');
            }
        }
        else {
            uart_putc(*format);
        }

        ++format;
    }
    va_end(args);
}

void uart_putc(char c) {
    // While hardware is being used - chekc bit 5 - if its 0, its occupied...
    while ((*LSR & 0x20) == 0) {
        // Poll status register bit 5 (Transmitter Holding Register Empty)
    }
    // When its free write the char to the free register
    *THR = c;
}

void uart_puts(const char* str) {
    while (*str != '\0') {
        uart_putc(*str);
        str++;
    }
}