#ifndef PARSE_H
#define PARSE_H

#include "getword.h"

#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAXITEM 100
#define MAXCMD 2

typedef struct child {
    /* Argument vector information */
    char buf[STORAGE * MAXITEM];  /* Where the actual chars of childargv are stored */
    char *childargv[MAXITEM];     /* argv for child process */

    struct child *next;           /* Next command in the pipeline */
} child_t;

typedef struct cmd {
    /* Pipeline-wide flags */
    bool foundeof;                /* Reached EOF or the word logout while parsing */
    bool runinbg;                 /* Run cmd in background */
    bool clobber;                 /* Overwrite a redirected stdout file if it exists */
    char cmdstdin[MAXNAMLEN];     /* Filename of child processes stdin */
    char cmdstdout[MAXNAMLEN];    /* Filename of child processes stdout */

    child_t fstchild;             /* Linked list of processes to run for the command */
} cmd_t;

/* Parse error codes */
typedef enum {
    noerror = 0,                  /* No error occurred */
    noquote,                      /* Expected closing single quote (`'`) */
    nofile,                       /* Expected file name */
    badfile,                      /* Found meta character instead of file name */
    nocmd,                        /* Expected executable name */
    nomem,                        /* Could not allocated space for a command */
    dupredir,                     /* Encountered multiple input / output redirection files */
    argvsize,                     /* Command had too many words to fit into a child's argv (number of words > MAXITEM) */
} parseerr_t;

/*
 * Global parse error code; reset after each call to parse
 */
extern parseerr_t parseerrno;

/*
 * Repeatedly calls `getword` to process input from `stdin`. Returns a
 * pointer to a command structure (`cmd_t`) or `NULL` on parse error.
 *
 * NOTE: Call `cmdfree` to free resources associated with the command.
 */
cmd_t *parse(void);

/*
 * Returns true if EOF or logout was encountered while parsing command;
 * otherwise returns false
 */
bool cmdeof(cmd_t *cmd);

/*
 * Returns true if the command is empty (no redirections, no children, no
 * globals set, etc.); otherwise, returns false
 */
bool isemptycmd(cmd_t *cmd);

/*
 * Frees resources associated with the given command
 */
void cmdfree(cmd_t *cmd);

/*
 * Print an error message associated with a parse error
 */
void pparseerr(void);

#endif /* PARSE_H */
