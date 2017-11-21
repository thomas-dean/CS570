#include "builtins.h"

static char *builtins[] = {
    "ls-F",
    "cd",
    "exec",
    NULL
};

static void display(char *filename);
static int ls(int argc, char *argv[]);
static int cd(char *dir);
static void exec(int argc, char *argv[]);

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
            return -1;
        }
        return cd(child->childargv[1]);
    }
    if (strcmp(child->buf, "ls-F") == 0) {
        return ls(argc, child->childargv);
    }
    if (strcmp(child->buf, "exec") == 0) {
        exec(argc, child->childargv);
    }
    fprintf(stderr, "Internal error: failed to find builtin for %s\n", child->buf);
    return -1;
}

static void display(char *filename)
{
    struct stat sb;
    struct stat lsb;

    if (lstat(filename, &lsb) == -1) {
        perror(filename);
        return;
    }
    if (S_ISDIR(lsb.st_mode)) {
        printf("%s/\n", basename(filename));
    } else if (S_ISLNK(lsb.st_mode)) {
        if (stat(filename, &sb) == -1) {
            /* lstat succeeded, but stat failed -> broken link */
            printf("%s&\n", basename(filename));
        } else {
            printf("%s@\n", basename(filename));
        }
    } else if (lsb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        printf("%s*\n", basename(filename));
    } else {
        printf("%s\n", basename(filename));
    }
}

static int ls(int argc, char *argv[])
{
    DIR *entry;
    struct dirent *dp;
    struct stat sb;
    char path[PATH_MAX + 1];

    if (argc == 1) {
        if ((entry = opendir(".")) == NULL) {
            perror(".");
            return EOPEN;
        }
        while ((dp = readdir(entry)) != NULL) {
            /* Current directory -> we don't need to construct the full path */
            display(dp->d_name);
        }
        if (closedir(entry) == -1) {
            perror("closedir");
            return ECLOSE;
        }
    }
    argv++; /* move past argv[0] which should always be 'ls-F' */

    while (*argv != NULL) {
        char *file = *argv++;

        if (lstat(file, &sb) == -1) {
            perror(file);
            continue;
        }
        if (S_ISDIR(sb.st_mode)) {
            if ((entry = opendir(file)) == NULL) {
                perror(file);
                return EOPEN;
            }
            while ((dp = readdir(entry)) != NULL) {
                strncpy(path, file, PATH_MAX);
                strncat(path, "/", 1);
                /*
                 * This should really be PATH_MAX - strlen(file), but file is at
                 * most STORAGE which is 255 and the max length for the d_name
                 * field is 255 and PATH_MAX is usually 4096. So we will not
                 * overrun our buffer since 4096 > 255 + 255.
                 */
                strncat(path, dp->d_name, PATH_MAX);
                display(path);
            }
            if (closedir(entry) == -1) {
                perror("closedir");
                return ECLOSE;
            }
        } else {
            /* stat succeeded + a non-directory -> just print the filename */
            display(file);
        }
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

static void exec(int argc, char *argv[])
{
    char **newargv;

    newargv = argv + 1;
    execvp(newargv[0], newargv);
}
