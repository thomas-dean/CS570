#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "getword.h"

#define NUM_INPUTS 15

static char* inputs[NUM_INPUTS] = {
    "foo bar baz bang",
    "this is a test",
    "empty string coming up...",
    "\n",
    "	leading tab...	tabs in the middle...	",
    "that last one had a trailing tab",
    "this one has a new line...\n",
    "Goodbye!",
    "Null&Void",
    "Null\\&Void",
    "Null\\ Void",
    "continued on the next \\\n  line",
    "Catch EOF with trailing \\",
    "foo logout",
    "test with;semicolon"
};

static char* expected_words[NUM_INPUTS][8] = {
    {"foo", "bar", "baz", "bang", ""}, // 5
    {"this", "is", "a", "test", ""}, // 5
    {"empty", "string", "coming", "up...", ""}, // 5
    {"", ""}, // 2
    {"leading", "tab...", "tabs", "in", "the", "middle...", ""}, // 7
    {"that", "last", "one", "had", "a", "trailing", "tab", ""}, // 8
    {"this", "one", "has", "a", "new", "line...", "", ""}, // 8
    {"Goodbye!", ""}, // 2
    {"Null", "&", "Void", ""}, // 4
    {"Null&Void", ""}, // 2
    {"Null Void", ""}, // 2
    {"continued", "on", "the", "next", "line", ""}, // 6
    {"Catch", "EOF", "with", "trailing", "\\", ""}, // 6
    {"foo", "logout", ""}, // 3
    {"test", "with", ";", "semicolon", ""} // 5
};

static int expected_counts[NUM_INPUTS][8] = {
    {3, 3, 3, 4, -1},
    {4, 2, 1, 4, -1},
    {5, 6, 6, 5, -1},
    {0, -1},
    {7, 6, 4, 2, 3, 9, -1},
    {4, 4, 3, 3, 1, 8, 3, -1},
    {4, 3, 3, 1, 3, 7, 0, -1},
    {8, -1},
    {4, 1, 4, -1},
    {9, -1},
    {9, -1},
    {9, 2, 3, 4, 4, -1},
    {5, 3, 4, 8, 1, -1},
    {3, -1, -1},
    {4, 4, 1, 9, -1}
};

static uint32_t expected_calls[NUM_INPUTS] = {
    5, 5, 5, 2, 7, 8, 8, 2, 4, 2, 2, 6, 6, 3, 5
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
static int try_input(int counts[], char *words[],
        uint32_t num_expected_calls)
{
    uint32_t i;
    int rv;
    char s[STORAGE];

    for (i = 0; i < num_expected_calls; ++i) {
        rv = getword(s);
        if (rv != counts[i]) {
            fprintf(stderr, "getword returned %d, expected %d\n", rv,
                    counts[i]);
            return 1;
        }
        if (strcmp(s, words[i]) != 0) {
            fprintf(stderr, "getword found %s, expected %s\n", s,
                    words[i]);
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
