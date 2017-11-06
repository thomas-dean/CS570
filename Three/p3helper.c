/*
 * p3helper.c
 * Thomas Dean
 * Program 3
 * CS570
 * Carroll
 * SDSU
 * November 3, 2017
 */
#include "p3.h"

/*
 * General documentation for the following functions is in p3.h
 */
void initstudentstuff(int protocol)
{
}

void prolog(int kind, int protocol)
{
    if (kind == READER) {
        if (protocol == FAIR) {
            /* Do fair reader prolog */
        } else if (protocol == WRIT) {
            /* Do writer biased prolog for reader */
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else if (kind = WRITER) {
        if (protocol == FAIR) {
            /* Do fair writer prolog */
        } else if (protocol == WRIT) {
            /* Do writer biased prolog for writer */
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else {
        fprintf(stderr, "Unknown access type, expected READER or WRITER\n");
    }
}

void epilog(int kind, int protocol)
{
    if (kind == READER) {
        if (protocol == FAIR) {
            /* Do fair reader epilog */
        } else if (protocol == WRIT) {
            /* Do writer biased epilog for reader */
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else if (kind = WRITER) {
        if (protocol == FAIR) {
            /* Do fair writer epilog */
        } else if (protocol == WRIT) {
            /* Do writer biased epilog for writer */
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else {
        fprintf(stderr, "Unknown access type, expected READER or WRITER\n");
    }
}
