#include "getword.h"

int main()
{
    int c;
    char s[STORAGE];

    for(;;) {
        (void) printf("n=%d, s=[%s]\n", c = getword(s), s);
        if (c == -1) {
            break;
        }
    }
}
