#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "getword.h"

#define MAX_ITERATIONS 1000
#define NUM_INPUTS 8

static char* inputs[NUM_INPUTS] = {
    "foo bar baz bang",
    "this is a test",
    "empty string coming up...",
    "\n",
    "	leading tab...	tabs in the middle...	",
    "that last one had a trailing tab",
    "this one has a new line...\n",
    "Goodbye!"
};

static char* expected_words[NUM_INPUTS][100] = {
    {"foo bar baz bang", ""},
    {"this is a test", ""},
    {"empty string coming up...", ""},
    {"", ""},
    {"leading tab...", "tabs in the middle...", ""},
    {"that last one had a trailing tab", ""},
    {"this one has a new line...", "", ""},
    {"Goodbye!", ""}
};

static int expected_counts[NUM_INPUTS][100] = {
    {16, -1},
    {14, -1},
    {25, -1},
    {0, -1},
    {14, 21, -1},
    {32, -1},
    {26, 0, -1},
    {8, -1}
};

static int expected_calls[NUM_INPUTS] = {
    2, 2, 2, 2, 3, 2, 3, 2
};

/*
 * calls getword `num_expected_words` times
 *
 * if getword produces the expected words and counts for all calls
 *  returns 0
 * if getword produces an unexpected value
 *  prints the expected and actual
 *  returns 1
 */
static int try_input(int expected_counts[], char *expected_words[],
        int num_expected_calls)
{
    uint32_t i;
    int rv;
    char s[STORAGE];

    for (i = 0; i < num_expected_calls; ++i) {
        rv = getword(s);
        if (rv != expected_counts[i]) {
            fprintf(stderr, "getword returned %d, expected %d\n", rv,
                    expected_counts[i]);
            return 1;
        }
        if (strcmp(s, expected_words[i]) != 0) {
            fprintf(stderr, "getword found %s, expected %s\n", s,
                    expected_words[i]);
            return 1;
        }
    }
    return 0;
}

static void mkstdin(char *input)
{
    /* WARNING: Closing and reassigning stdin is not portable */
    /* The fix for this is to write `input` to a file, then freopen that file as
     * stdin */
    fclose(stdin);
    stdin = fmemopen(input, strlen(input), "r");
}

int main(void)
{
    uint32_t i;
    int rv;
    int failed = 0;

    for (i = 0; i < NUM_INPUTS; i++) {
        mkstdin(inputs[i]);
        rv = try_input(expected_counts[i], expected_words[i],
                expected_calls[i]);
        if (rv != 0) {
            fprintf(stderr, "getword failed for input \"%s\"\n", inputs[i]);
            failed = 1;
        }
    }

    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
