#include "exec.h"
#include "parse.h"

#include <signal.h>

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
                break;
            }
            cmdfree(cmd);
            continue;
        }

        execcmd(cmd);

        if (cmdeof(cmd)) {
            break;
        }
        cmdfree(cmd);
    }

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
    sigemptyset(&sighandle.sa_mask);
    sigaddset(&sighandle.sa_mask, SIGTERM);
    sighandle.sa_flags = SA_RESTART;
    sighandle.sa_handler = SIG_IGN;

    setpgid(0, getpid());
    if (sigaction(SIGTERM, &sighandle, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }
}
