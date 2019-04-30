#include <stdio.h>
#include "cthread.h"

#define NUM_THREADS 5

void *th0(void *arg)
{
    (void)arg;
    printf("If this gets printed, run to the hills!\n");
    return 0;
}

int main()
{
    int tids[NUM_THREADS];
    printf("Main changing priority to 1...\n");
    csetprio(0, 1);
    printf("Creating %d threads with minimum priority 2...\n", NUM_THREADS);
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
    printf("Now main will cyield() once and, since it has higher priority than other threads, none will execute\n");
    cyield();
    printf("That's all folks!\n");
    return 0;
}

