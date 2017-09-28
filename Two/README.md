# CS570 - Program Two

This project consists of a few files:

* getword.c
* getword.h
* makefile
* p2.c
* builtins.c
* builtins.h
* exec.c
* exec.h
* parse.c
* parse.h
* redir.c
* redir.h

### getword.c

The C source file which contains the logic for parsing a word from `stdin`.

### getword.h

Header file for `getword`. Contains function prototype definitions, includes
and macros.

### makefile

Contains instructions for how to compile various portions of the project. It
also contains targets for creating a `tags` file and linting.

Available targets:

* p2
* a .o target for each .c source file
* clean
  - Removes files which are created by the makefile
* test
  - Runs various tests for `getword`
* tags
  - Creates a `tags` file which is used to jump to function and macro
  definitions among other things. See `ctags(3)`.
* lint
  - Runs clang's linter of all source files

### p2.c

This is the main executable file. It is essentially a very simple shell. Handles
pipelines, redirection and a couple of built-in commands.

### builtins.c

Contains the logic for the built-in commands as well as the interface used by
other source files. Namely `isbuiltin` and `runbuiltin`.

### builtins.h

Header file for `builtins`. Contains function prototype definitions, includes
and macros.

### exec.c

The meat of the program. Takes a parsed command and runs it. This may include
`fork(2)`ing and `exec(2)`ing a child process, running a built-in, or a
combination of both within the context of a pipeline.

### exec.h

Header file for `exec`. Contains function prototype definitions, includes
and macros.

### parse.c

Contains the logic for taking words from `stdin` (via `getword`) and converting
it into the data structure `cmd_t`. This data structure contains all the
required information for running a command.

It also reports errors when a malformed command is read. See `parseerrno` for
which error occurred.

### parse.h

Header file for `parse`. Contains function prototype definitions, includes
and macros.

### redir.c

Handles redirection of a command into and out of files.

### redir.h

Header file for `redir`. Contains function prototype definitions, includes
and macros.

