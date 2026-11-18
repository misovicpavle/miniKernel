set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Skip executable link tests during CMake configuration
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Absolute pathways to the cross-compiler binaries
set(CMAKE_C_COMPILER /usr/bin/riscv64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/riscv64-linux-gnu-g++)

set(CMAKE_CXX_FLAGS_INIT "-ffreestanding -nostdlib -std=c++23")