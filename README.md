# RISC-V Preemptive Kernel

A small educational kernel for the RISC-V architecture, implemented in C, C++, and Assembly. The project covers thread lifecycle management, preemptive scheduling, synchronization, system calls, console I/O, and low-level context switching.

## Highlights

- Preemptive thread scheduling driven by timer interrupts
- Time slicing and sleeping-thread management
- C and C++ system-call APIs
- Semaphore-based synchronization and producer-consumer coordination
- Custom dynamic memory allocator and global C++ `new`/`delete` integration
- Supervisor trap handling and user/supervisor execution modes
- Context switching implemented in RISC-V Assembly
- Automated kernel tests executed under QEMU

## Architecture

| Component | Responsibility |
|---|---|
| `src/tcb.cpp` | Thread control blocks, lifecycle, dispatching, and blocking |
| `src/scheduler.cpp` | Runnable-thread queue and scheduling decisions |
| `src/_sem.cpp` | Semaphore wait/signal behavior and blocked-thread management |
| `src/MemoryAllocator.cpp` | Kernel-side dynamic memory allocation |
| `src/riscv.cpp` | RISC-V control and status registers and interrupt handling |
| `src/supervisorTrap.S` | Supervisor trap entry and register preservation |
| `src/contextSwitch.S` | Low-level thread context switching |
| `src/syscall_c.cpp` | C system-call interface |
| `src/syscall_cpp.cpp` | Object-oriented C++ API for threads and semaphores |
| `src/_console.cpp` | Interrupt-aware console input/output |

## Public APIs

The kernel exposes both procedural C and object-oriented C++ interfaces.

```cpp
class Thread {
public:
    int start();
    static void dispatch();
    static int sleep(time_t duration);
};

class Semaphore {
public:
    int wait();
    int signal();
};
```

The implementation also overloads global `new`, `new[]`, `delete`, and `delete[]` so kernel-side C++ objects use the custom allocator.

## Build and run

The included Dockerfile provides the RISC-V cross-compiler, QEMU, GDB, and build tools.

```bash
docker build -t riscv-preemptive-kernel .
docker run --rm -it -v "$PWD:/app" -w /app riscv-preemptive-kernel bash
```

Inside the container:

```bash
make
make qemu
```

For a GDB session:

```bash
make qemu-gdb
```

Then connect from another terminal using `gdb-multiarch` and the port printed by the build script.

## Tests

The `test/` directory contains scenarios for:

- C and C++ thread APIs
- Thread sleeping and timer wakeups
- C and C++ producer-consumer synchronization
- Semaphore behavior
- User/supervisor mode transitions
- Resource cleanup and repeated thread execution

## Repository layout

```text
.
├── h/          # Kernel and public API headers
├── src/        # C, C++, and RISC-V Assembly implementation
├── test/       # Kernel integration tests
├── lib/        # Course-provided hardware support libraries
├── Dockerfile  # Reproducible RISC-V/QEMU toolchain
├── Makefile    # Build, QEMU, and GDB targets
└── kernel.ld   # Kernel linker script
```

## Technical takeaways

This project strengthened my understanding of interrupt-driven execution, context switching, synchronization, memory ownership, kernel/user boundaries, and debugging freestanding code under QEMU/GDB.

## License and attribution

The repository includes the original license and attribution for the educational RISC-V/xv6-derived support code. See [`LICENSE`](LICENSE) for details.
