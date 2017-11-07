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
 * Globals used for writer biased solution
 */
static int rcount, wcount;
static sem_t wlock, rlock, wcountlock, rcountlock, renterlock;

/*
 * Globals used for fair solution
 */
static sem_t mtx, access_pool;

/*
 * General documentation for the following functions is in p3.h
 */
void initstudentstuff(int protocol)
{
    if (protocol == FAIR) {
        pCHK(sem_init(&mtx, 0, 1));
        pCHK(sem_init(&access_pool, 0, MAXCUSTOMERS));
    } else if (protocol == WRIT) {
        pCHK(sem_init(&wlock, 0, 1));
        pCHK(sem_init(&rlock, 0, 1));
        pCHK(sem_init(&wcountlock, 0, 1));
        pCHK(sem_init(&rcountlock, 0, 1));
        pCHK(sem_init(&renterlock, 0, 1));
    } else {
        fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
    }
}

void prolog(int kind, int protocol)
{
    int i;

    if (kind == READER) {
        if (protocol == FAIR) {
            /* Do fair reader prolog */
            pCHK(sem_wait(&access_pool));
        } else if (protocol == WRIT) {
            /* Do writer biased prolog for reader */
            pCHK(sem_wait(&renterlock));
            pCHK(sem_wait(&rlock));
            pCHK(sem_wait(&rcountlock));
            if (++rcount == 1) {
                /* I am the first reader to come along, let's wait for the
                 * writers to clear up... */
                pCHK(sem_wait(&wlock));
            }
            pCHK(sem_post(&rcountlock));
            pCHK(sem_post(&rlock));
            pCHK(sem_post(&renterlock));
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else if (kind == WRITER) {
        if (protocol == FAIR) {
            /* Do fair writer prolog */
            /* Block out any new writers from trying to saturate pool; they will
             * be next in line after I finish acquiring all the locks */
            pCHK(sem_wait(&mtx));
            /* Saturate the pool of potential readers / writers; this will block
             * out any new readers and I will pick up the locks as old readers
             * finish */
            for (i = 0; i < MAXCUSTOMERS; ++i) {
                pCHK(sem_wait(&access_pool));
            }
        } else if (protocol == WRIT) {
            /* Do writer biased prolog for writer */
            pCHK(sem_wait(&wcountlock));
            if (++wcount == 1) {
                /* I am the first writer to come along, let's wait for the
                 * remaining readers to finish */
                pCHK(sem_wait(&rlock));
            }
            pCHK(sem_post(&wcountlock));
            pCHK(sem_wait(&wlock));
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else {
        fprintf(stderr, "Unknown access type, expected READER or WRITER\n");
    }
}

void epilog(int kind, int protocol)
{
    int i;

    if (kind == READER) {
        if (protocol == FAIR) {
            /* Do fair reader epilog */
            pCHK(sem_post(&access_pool));
        } else if (protocol == WRIT) {
            /* Do writer biased epilog for reader */
            pCHK(sem_wait(&rcountlock));
            if (--rcount == 0) {
                /* I was the last reader to leave, let's allow a writer to enter */
                pCHK(sem_post(&wlock));
            }
            pCHK(sem_post(&rcountlock));
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else if (kind == WRITER) {
        if (protocol == FAIR) {
            /* Do fair writer epilog */
            /* Give all the required locks back so others can enter */
            for (i = 0; i < MAXCUSTOMERS; ++i) {
                pCHK(sem_post(&access_pool));
            }
            /* Now that I am not holding any pooled locks I can let another
             * writer try to start to grab them. So, I release the writer lock */
            pCHK(sem_post(&mtx));
        } else if (protocol == WRIT) {
            /* Do writer biased epilog for writer */
            pCHK(sem_post(&wlock));
            pCHK(sem_wait(&wcountlock));
            if (--wcount == 0) {
                /* I was the last writer to leave, let's let a reader enter */
                pCHK(sem_post(&rlock));
            }
            pCHK(sem_post(&wcountlock));
        } else {
            fprintf(stderr, "Unknown protocol type, expected FAIR or WRIT\n");
        }
    } else {
        fprintf(stderr, "Unknown access type, expected READER or WRITER\n");
    }
}
