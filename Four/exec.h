#ifndef EXEC_H
#define EXEC_H

#include "builtins.h"
#include "parse.h"
#include "redir.h"

#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Run the command specified by `cmd`
 */
void execcmd(cmd_t *cmd);

#endif /* EXEC_H */
