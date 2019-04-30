#include <stdio.h>
#include "cthread.h"

#define NUM_THREADS 5

void *th0(void *arg)
{
    printf("th0 printing arg=%d\n", (int)arg);
    return 0;
}

int main()
{
    int tids[NUM_THREADS];
    printf("Creating %d threads with minimum priority 2 (same as main)...\n", NUM_THREADS);
    for(int i=0; i<NUM_THREADS; i++)
    {
        tids[i] = ccreate(th0, (void*)i, 2);
        if(tids[i] < 0)
        {
            printf("Thread creation error: %d\n", tids[i]);
            return -1;
        }
        else
            printf("th0 created with tid=%d and arg=%d\n", tids[i], i);
    }
    printf("Now main will cyield() once, which is enough for all threads to execute\n");
    cyield();
    printf("Main reporting back: That's all folks!\n");
    return 0;
}

