#include "builtins.h"

static char *builtins[] = {
    "ls-F",
    "cd",
    NULL
};

static int ls(const char *dir);
static int cd(char *dir);

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
        return ls(child->childargv[1]);
    }
    fprintf(stderr, "Internal error: failed to find builtin for %s\n", child->buf);
    return -1;
}

static int ls(const char *dir)
{
    DIR *entry;
    struct dirent *dp;
    struct stat sb;

    if (dir == NULL) {
        dir = ".";
    }
    if (stat(dir, &sb) == -1) {
        perror(dir);
        return EOPEN;
    }
    if (S_ISDIR(sb.st_mode)) {
        if ((entry = opendir(dir)) == NULL) {
            perror(dir);
            return EOPEN;
        }
        while ((dp = readdir(entry)) != NULL) {
            printf("%s\n", dp->d_name);
        }
        if (closedir(entry) == -1) {
            perror("closedir");
            return ECLOSE;
        }
    } else {
        /* stat succeeded + a non-directory -> just print the filename */
        printf("%s\n", dir);
    }
    return 0;
}

static int cd(char *dir) {
    if (dir == NULL) {
        dir = getenv("HOME");
        if (!dir) {
            fprintf(stderr, "Could not find environment variable $HOME\n");
            return ECHDIR;
        }
    }
    if (chdir(dir) == -1) {
        perror(dir);
        return ECHDIR;
    }
    return 0;
}
