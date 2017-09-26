#include "exec.h"
#include "parse.h"

#include <signal.h>

#define PGID 1234321

static void sigsetup(void);
#ifdef DEBUG
static void printcmddebug(cmd_t *cmd);
#endif


int main()
{
    cmd_t *cmd;

    sigsetup();
    while (true) {
        printf("p2: ");
        fflush(stdout);
        cmd = parse();
        if (cmd == NULL) {
            pparseerr();
            continue;
        }
        if (isemptycmd(cmd)) {
            if (cmdeof(cmd)) {
                goto cleanup;
            }
            cmdfree(cmd);
            continue;
        }

        /* Do something with cmd */
#ifdef DEBUG
        printcmddebug(cmd);
#endif
        execcmd(cmd);

        if (cmdeof(cmd)) {
            goto cleanup;
        }
        cmdfree(cmd);
    }

cleanup:
    if (cmd != NULL) {
        cmdfree(cmd);
    }
    killpg(getpgrp(), SIGTERM);
    printf("p2 terminated.\n");
    return 0;
}

static void sigsetup(void)
{
    struct sigaction sighandle;
    sighandle.sa_flags = SA_RESTART;
    sighandle.sa_handler = SIG_IGN;

    setpgid(0, PGID);
    if (sigaction(SIGTERM, &sighandle, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
}

#ifdef DEBUG
static void printcmddebug(cmd_t *cmd)
{
    int i;
    char **currarg;
    child_t *currchild;

    if (isemptycmd(cmd)) {
        return;
    }
    currchild = &cmd->fstchild;
    i = 0;
    printf("=========================== DEBUG INFO ===========================\n");
    printf("---------------- CMD DETAILS -----------------\n");
    printf("\tfoundeof = %s\n", cmd->foundeof ? "true" : "false");
    printf("\truninbg = %s\n", cmd->runinbg ? "true" : "false");
    printf("\tclobber = %s\n", cmd->clobber ? "true" : "false");
    printf("\tcmdstdin = %s\n", cmd->cmdstdin);
    printf("\tcmdstdout = %s\n", cmd->cmdstdout);
    printf("---------------- CHILD DETAILS -----------------\n");
    while (currchild != NULL) {
        i++;
        currarg = currchild->childargv;
        printf("\t---------------- CHILD NUMBER %d DETAILS -----------------\n", i);
        printf("\t\tbuf = [%s]\n", currchild->buf);
        printf("\t\tchildargv:\n");
        while (*currarg != NULL) {
            printf("\t\t\t[%s]\n", *currarg++);
        }
        printf("\t\tnext = %p\n", (void *)currchild->next);
        currchild = currchild->next;
    }
    if (cmdeof(cmd)) {
        printf("==================== Reached EOF ====================\n");
    }
    printf("========================= END DEBUG INFO =========================\n");
}
#endif
