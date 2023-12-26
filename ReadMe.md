# Full-System Emulator

## Overview

This project is a Full-System Emulator developed using C(mainly) and C++. The emulator is designed to simulate various aspects of a computer system, providing a comprehensive understanding of computer architecture. The project includes the following key features:

- **Arithmetic and Logical Operations:** The emulator simulates arithmetic and logical operations on both integers and floating-point numbers. It implements the instruction fetch, decode, and execute processes, enhancing CPU functionality.

- **ELF Loading:** Developed ELF loading functionality for executable files, ensuring compatibility and proper execution within the emulator.

- **Memory Simulation:** Simulated registers and main memory, implementing a paging mechanism to translate virtual addresses. Additionally, the project includes the simulation of cache and addressing in protected mode for memory optimization.

- **Debugger Functionality:** Implemented a debugger with expression evaluation using lexical analysis and recursive evaluation. This feature allows for obtaining runtime information of variables and functions through a symbol table, enhancing the debugging experience.

- **Exception Handling:** The project includes handling responses to exceptions within the trap class and addressing interrupts to ensure robust error handling.

- **I/O Operations:** Simulated input/output operations, contributing to a more realistic emulation of a computer system.
