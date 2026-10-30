extern "C" void trap_vector();

void trap_init() {
    asm volatile("csrw stvec, %0" : "r"(trap_vector));
}