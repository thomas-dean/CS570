#include "builtins.h"

static char *builtins[] = {
    "ls-F",
    "cd",
    NULL
};

static int ls(int outfd, const char *dir)
{
    DIR *entry;
    struct dirent *dp;
    int outcp, i;
    FILE *stream;

    if (dir == NULL) {
        dir = ".";
    }
    if ((outcp = dup(outfd)) == -1) {
        perror("dup");
        return EOPEN;
    }
    if ((stream = fdopen(outcp, "w")) == NULL)  {
        perror("fdopen");
        if (close(outcp) == -1) {
            perror("close");
            return ECLOSE;
        }
        return EOPEN;
    }
    if ((entry = opendir(dir)) == NULL) {
        perror(dir);
        return EOPEN;
    }
    i = 0;
    while ((dp = readdir(entry)) != NULL) {
        fprintf(stream, "%s\n", dp->d_name);
        ++i;
    }
    if (closedir(entry) == -1) {
        perror("closedir");
        return ECLOSE;
    }
    if (fclose(stream) != 0) {
        perror("fclose");
        return ECLOSE;
    }
    return i;
}

static int cd(char *dir) {
    if (dir == NULL) {
        dir = getenv("HOME");
    }
    if (chdir(dir) == -1) {
        perror(dir);
        return ECHDIR;
    }
    return 0;
}

bool isbuiltin(char *exename)
{
    char **bp;

    for (bp = builtins; *bp; ++bp) {
        if (strcmp(*bp, exename) == 0) {
            return true;
        }
    }
    return false;
}

int runbuiltin(child_t *child)
{
    int argc;

    for (argc = 0; child->childargv[argc]; ++argc) {
        ;
    }
    if (strcmp(child->buf, "cd") == 0) {
        if (argc > 2) {
            fprintf(stderr, "Too many arguments to cd\n");
        }
        return cd(child->childargv[1]);
    }
    if (strcmp(child->buf, "ls-F") == 0) {
        if (argc > 2) {
            fprintf(stderr, "Too many arguments to ls-F\n");
        }
        /* We have already setup stdout for redirection */
        return ls(STDOUT_FILENO, child->childargv[1]);
    }
    fprintf(stderr, "Internal error: failed to find builtin for %s\n", child->buf);
    return -1;
}
