#include "include/uart.h"

/* 
    sepc -  supervisor exception program  counter - holds 64b virtual address of
            the instruction that caused the exception
    scause - supervisor cause - Bit 63 = 1 (Interrupt), = 0 (Exception)
                                Bit 62:0 = Numerical cause ID
                                            13 = Load Page Fault
                                            9 = ecall ...
    stval - supervisor trap value - Holds data that was involved in failures
            For page faults - the virutal address, for illegal instructions - the instruction opcode
    sstatus - supervisor status - what state was the cpu in?  
                SPP Bit 8: Previous privlege mode(0=User, 1=Supervisor)
                SPIE Bit 5: Interrupt enable state before trap
                SIE Bit 1; Current supervisor interrupt enable bit
    */
    /* %0 - any register hardware chooses(e.g. a0) - like for printf() the %d for ints*/
    /* =r - write to a cpp variable only, +r = read this register and write back to it, read only - no prefix t*/


inline uint64_t r_sepc() {
    uint64_t sepc;
    asm volatile("csrr %0, sepc" : "=r"(sepc));
    return sepc;
}
inline uint64_t r_scause() {
    uint64_t scause;
    asm volatile("csrr %0, sepc" : "=r"(scause));
    return scause;
}
inline uint64_t r_stval() {
    uint64_t stval;
    asm volatile("csrr %0, sepc" : "=r"(stval));
    return stval;
}
inline uint64_t r_sstatus() {
    uint64_t sstatus;
    asm volatile("csrr %0, sepc" : "=r"(sstatus));
    return sstatus;
}

extern "C" void handle_trap() noexcept {
    uint64_t sepc, scause, stval, sstatus;
    sepc = r_sepc();
    //invalid
    
    //async interrupt - 1 OR sync exception - 0
    uint64_t causeBit = (scause >> 63);
    uint64_t errorCode = scause & ~(1ULL << 63);
    switch(causeBit) {
        case 1: //bit is 1 - async interrupt
            switch(errorCode) {
                case 1:
                    break;
                case 5:
                    break;
                case 9:
                    break;
                case 13:
                    break;
                default: return;
            }
            break;
        case 0: //bit is 0 - sync exception
            switch(errorCode) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                    break;
                case 9:
                    break;
                case 12:
                    break;
                case 13:
                    break;
                case 15:
                    break;
                default: return;
            }
            break;
        default: return;
    }
}
