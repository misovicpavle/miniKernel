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
    /*
    asm volatile (
    "assembly code" // Template string
    : outputs       // 1st colon: Writes assembly register values into C++ variables. r no prefix, +r, =r
                    +r - takes cpp variable value, loads into a asm register, asm modifies its value, and updates the cpp variable
                    =r - overwrites the cpp variable with the asm register value
                    noprefix r - only read my cpp variable, dont write anything back to my cpp variable
    : inputs        // 2nd colon: Loads C++ variables into registers for assembly to read. only r
    : clobbers      // 3rd colon: Tells GCC which registers/memory were overwritten.
    );
    */

inline uint64_t r_sepc() {
    uint64_t sepc;
    asm volatile("csrr %0, sepc" : "=r"(sepc));
    return sepc;
}
inline uint64_t r_scause() {
    uint64_t scause;
    asm volatile("csrr %0, scause" : "=r"(scause));
    return scause;
}
inline uint64_t r_stval() {
    uint64_t stval;
    asm volatile("csrr %0, stval" : "=r"(stval));
    return stval;
}
inline uint64_t r_sstatus() {
    uint64_t sstatus;
    asm volatile("csrr %0, sstatus" : "=r"(sstatus));
    return sstatus;
}

enum class IPI_CODES { TLB_FLUSH, SHUTDOWN,  };

/* This is a consumer function - the buffer IPI is filled by other syscalls
    which at the time might or might not be written yet. This simply reads the buffer
    and does what is instructed by other syscalls*/
extern "C" void handle_trap() noexcept {
    uint64_t sepc, scause, stval, sstatus;
    sepc = r_sepc();
    //invalid
    
    //core hart-id read, register 'tp'

    //async interrupt - 1 OR sync exception - 0
    uint64_t isInterrupt = (scause >> 63);
    uint64_t errorCode = scause & ~(1ULL << 63);
    switch(isInterrupt) {
        case 1: //bit is 1 - async interrupt
            /* READING THE SIP REGISTER - SUPERVISOR INTERRUPT PENDING */
            /* its triggered when a interrupt happens - we inspect/clear SIP: */
            uint64_t SIP;
            asm volatile("csrr %0, sip" : : "=r"(SIP)); 
            switch(errorCode) {
                case 1: /* Supervisor Software Interrupt Pending - SSIP */
                    uint64_t newSIP = SIP & ~(1ULL << 1); // reset SSIP in SIP
                    asm volatile("csrw sip, %0" : : "r"(newSIP)); //write it back
                    //read inter core message: tlb flush, thread reschedule, shutdown signal from another core?
                    //Inter-Processor communication(IPI) - IPI message queue
                    
                    //sret resume execution
                    break;
                case 5: /* Supervisor Timer Interrupt */
                    break;
                case 9: /* Supervisor External Interrupt */
                    break;
                default: break;
            }
            break;
        case 0: //bit is 0 - sync exception
            switch(errorCode) {
                case 8: /* Syscall from U-mode */
                    break; 
                case 9: /* Syscall from S-mode */
                    break;
                case 12: /* Instruction Page Fault */
                    break;
                case 13: /* Load Page Fault */
                    break;
                case 15: /* Store Page Fault */
                    break;
                default: /* 0-7 - default to fatal cpu exception*/
                    break;
            }
            break;
        default: return;
    }
}
