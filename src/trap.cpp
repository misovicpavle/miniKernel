extern "C" void trap_vector(); //function symbol - read only

/* %0 - any register hardware chooses(e.g. a0) - like for printf() the %d for ints*/
/* =r - write only, +r = read this register and write back to it, read only - no prefix t*/
void trap_init() {
    asm volatile("csrw stvec, %0" : : "r"(trap_vector));
}

