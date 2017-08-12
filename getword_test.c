#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "getword.h"

#define MAX_ITERATIONS 1000

int main(void)
{
    uint32_t i;
    int rv;
    char s[STORAGE];

    if (freopen("data/input1.txt", "r", stdin) == NULL) {
        err(EXIT_FAILURE, "data/input1.txt");
    }
    for (i = 0; i < MAX_ITERATIONS; ++i) {
        rv = getword(s);
        printf("rv=[%d]s=\"%s\"\n", rv, s);
        if (rv == -1) {
            /* Reached EOF */
            if (!feof(stdin)) {
                fprintf(stderr, "getword failed to reach EOF\n");
                return EXIT_FAILURE;
            }
            break;
        }
    }
    if (i == MAX_ITERATIONS) {
        fprintf(stderr, "reached MAX_ITERATIONS (%d) without seeing EOF\n", MAX_ITERATIONS);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
