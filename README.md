# SisOp 2019-1 T1 #
Trabalho 1 de Sistemas Operacionais I (2019/1)

## Overview ##

This is the first (of 2) projects of the Operating Systems I course of Computer Science graduation at UFRGS.

The goal is to provide a simple library for dealing with threads, in which the user can do some things, such as:
1. Create new threads
2. Wait until a thread finishes execution
3. Change the priority of the running thread
4. Create and use semaphores to simulate shared resources
5. Yield execution

The proposed scheduler is a non-preemptive priority FIFO. Although it supports preemptivity by changing the PREEMPTIVE to 1.

## Instructions ##

To compile the library `libcthreads.a`, enter `make all` in the root directory of the repository.

Alternatively, to compile for x86_64, enter `make all64`. That will use the student-made `bin/support.c` source instead of the professor-provided `bin/support.o`.
There is no guarantee `bin/support.c` has `bin/support.o`'s functionalities fully implemented and 100% correct.

To compile all the programs inside `exemplos/` or `testes/`, you can enter `make all` inside the desired directory.

Alternatively, you can compile the programs of your choice by entering `make this_one`, having a `this_one.c` file in the directory.

The command `make clean` inside each directory cleans exactly what the command `make all` (plus `make all64` in the root folder) creates.

More information is available (in Portuguese) in the files inside the `material/` folder.

## Authorship ##

Files created (not provided):

1. `src/*`
2. `testes/*`
3. `Makefile`s
4. `bin/support.c`

By:

- Yuri Jaschek <<yuri.jaschek@inf.ufrgs.br>>
- Giovane Fonseca
- Matheus F. Kovaleski
