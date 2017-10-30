# CS570 - Program Three

This project consists of a few files:

* p3helper.c
* p3main.c
* CHK.h
* p3.h
* makefile

### p3helper.c

The logic for dealing with the readers and writters.

### p3main.c

The main executable of the program.

### CHK.h

A collection of macros for checking for errors from system calls and pthread
related functions.

### p3.h

Main program header file.

### makefile

Contains instructions for how to compile various portions of the project. It
also contains targets for creating a `tags` file and linting.

Available targets:

* p3
* p3main.o
* p3helper.o
* clean
  - Removes files which are created by the makefile
* test
  - Runs a test for p3
* test2
  - Runs another test for p3
* tags
  - Creates a `tags` file which is used to jump to function and macro
  definitions among other things. See `ctags(3)`.
* lint
  - Runs clang's linter of all source files
