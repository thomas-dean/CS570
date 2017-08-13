# CS570 (Operating Systems)

CS570 is the operating systems course at San Diego State University (SDSU).

This project consists of a few files:

* getword.c
* getword.h
* getword\_test.c
* makefile
* p0.c

### getword.c

The C source file which contains the logic for parsing a word from `stdin`.

### getword.h

Header file for `getword`. Contains function prototype definitions, includes
and macros.

### getword\_test.c

Runs tests to verify `getword` is working as intended.

### makefile

Contains instructions for how to compile various portions of the project. It
also contains targets for creating a `tags` file and linting.

Available targets:

* p0
* getword\_test
* p0.o
* getword\_test.o
* getword.o
* clean
  - Removes files which are created by the makefile
* test
  - Runs various tests for `getword`
* tags
  - Creates a `tags` file which is used to jump to function and macro 
  definitions among other things. See `ctags(3)`.
* lint
  - Runs clang's linter of all source files

### p0.c

This is the main executable file. It contains the main function which will
repeatedly call `getword` to parse input from `stdin`.
