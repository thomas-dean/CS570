#include "builtins.h"

int ls(int outfd, const char *dir)
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

int cd(char *dir) {
    if (dir == NULL) {
        dir = getenv("HOME");
    }
    if (chdir(dir) == -1) {
        perror(dir);
        return ECHDIR;
    }
    return 0;
}
