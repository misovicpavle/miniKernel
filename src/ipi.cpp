#include <ipi.h>
#define NUM_CORES 8
#define IPI_QUEUE_SIZE 64

struct IPI_ENTRY {
    IPI_TYPE ipi_instr;
    uint64_t virtual_mem_addr;
};

struct HART_MAILBOX {
    uint64_t HART_ID;
    IPI_ENTRY IPI_BUFFER[IPI_QUEUE_SIZE];
    size_t IPI_HEAD = 0; //receiver reads here
    size_t IPI_TAIL = 0; //sender writes here
};

HART_MAILBOX CORES[NUM_CORES];
//BUG-FIX: uint32_t* lock = 0; is a nullptr, dereference is impossible, so just do this and pass it in as &
uint32_t lock = 0; //0 - free, 1 - locked

//FIFO - QUEUE - SINGLY LINKED LIST
//amoswap.w - spin lock atomic memory swap from RISC-V
//global turn flag = hart_id which is in register td

inline void lock_acquire(volatile uint32_t* lock) {
    while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE)) {
        //spin while its 1, if its 0 then it was free so you can proceed..
        //enter low-power mode
#if defined(__riscv) //if
        asm volatile("pause");
#endif
    }
}

//spinlock release
inline void lock_free(volatile uint32_t* lock) {
    __atomic_clear(lock, __ATOMIC_RELEASE);
}


bool add_ipi_instr(uint64_t target_hart_id, IPI_ENTRY NEW_ENTRY) { //append right - sender/producer
    //acquire lock
    lock_acquire(&lock);
    //check hart_id through tp
    uint64_t current_hart_id;
    //tp is a standard cpu gpr, not a csr, so no csrw, just mv
    asm volatile("mv %0, tp" : "=r"(current_hart_id));
    //loop through cores until you find core = hart_id
    for (size_t i = 0; i < NUM_CORES; i++) {
        if (CORES[i].HART_ID == target_hart_id) {
            //modify the cores ipi buffer - add instruction
            if ((CORES[i].IPI_TAIL + 1) % 64 != CORES[i].IPI_HEAD) {
                CORES[i].IPI_BUFFER[CORES[i].IPI_TAIL] = NEW_ENTRY;
                CORES[i].IPI_TAIL = (CORES[i].IPI_TAIL + 1) % 64; //move to the next instruction
            }
            else { 
                lock_free(&lock);
                return false; 
            } // queue is full
        }
    }
    //end
    lock_free(&lock);
    return true;
}

bool pop_ipi_instr(IPI_ENTRY* removed_entry) { //pop left - receiver/consumer
    //acquire lock
    lock_acquire(&lock);
    //check hart_id through tp
    uint64_t current_hart_id;
    asm volatile("mv %0, tp" : "=r"(current_hart_id));
    //loop through cores until you find core = hart_id
    for (size_t i = 0; i < NUM_CORES; i++) {
        if (CORES[i].IPI_HEAD == CORES[i].IPI_TAIL) { 
            lock_free(&lock);
            return false; 
        } //queue empty/none
        if (CORES[i].HART_ID == current_hart_id) {
            //modify the cores ipi buffer - pop instruction, mod 64 is a trick so if its 64 it circles to 0 again
            *removed_entry = CORES[i].IPI_BUFFER[CORES[i].IPI_HEAD];
            CORES[i].IPI_HEAD = (CORES[i].IPI_HEAD + 1) % 64; //move to the next instruction
        }
    }
    //end
    lock_free(&lock);
    return true;
} 