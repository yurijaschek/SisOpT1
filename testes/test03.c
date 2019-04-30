#include <stdio.h>
#include "cthread.h"

#define NUM_THREADS 5

void *th0(void *arg)
{
    int id = (int)arg;
    if(id < NUM_THREADS+1)
        cjoin(id);
    printf("I've just waited for thread %d\n", id);
    return 0;
}

int main()
{
    int tids[NUM_THREADS];
    printf("Creating %d threads with minimum priority 2 (same as main)...\n", NUM_THREADS);
    for(int i=0; i<NUM_THREADS; i++)
    {
        tids[i] = ccreate(th0, (void*)(i+2), 2); // Assumes they are incrementing starting with 1
        if(tids[i] < 0)
        {
            printf("Thread creation error: %d\n", tids[i]);
            return -1;
        }
        else
            printf("th0 created with tid=%d and arg=%d\n", tids[i], i+2);
    }
    printf("Now main will cjoin() on the first thread created, and they will cjoin() on the next, etc...\n");
    cjoin(tids[0]);
    printf("Main reporting back: That's all folks!\n");
    return 0;
}

