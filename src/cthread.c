/*****************************************************************************
 *  Instituto de Informática - Universidade Federal do Rio Grande do Sul     *
 *  INF01142 - Sistemas Operacionais I N                                     *
 *  Trabalho Prático 1 - compact thread (cthread)                            *
 *                                                                           *
 *  Nomes: 00243451 Giovane Fonseca                                          *
 *         00274744 Matheus F. Kovaleski                                     *
 *         00231592 Yuri Jaschek                                             *
 *                                                                           *
 *****************************************************************************/

#include "cthread.h"
#include "cdata.h"
#include "support.h"
#include <stdbool.h>
#include <stdio.h>

#define ARRAY_SIZE(x) (int)(sizeof(x) / sizeof((x)[0]))

#define STACK_SIZE  8192    // Stack size for each thread
#define NUM_PRIO    3       // Number of possible priorities (3 for 2019/1), from 0 to NUM_PRIO


/**********************
 *  Global variables  *
 **********************/

static int tid_counter = 1; // Incremental counter for thread ID (tid) assignments. Main will be 0

static PFILA2 able;         // Queue of threads able to be executed (selected by the scheduler)
static PFILA2 blocked;      // Queue of the blocked threads, awaiting an unblocking event
static TCB_t *executing;    // Thread control block (TCB) of the thread that is currently using the CPU


/**************************************
 *  Internal functions (not exposed)  *
 **************************************/

/*-----------------------------------------------------------------------------
Funct:  Initializes needed structures and creates the main thread, making the
        library usable. Subsequent calls after the first succeeded call always
        succeed.
Return: If the function succeeds, 0 will be returned. Otherwise -1.
-----------------------------------------------------------------------------*/
static int init_cthread(void)
{
    static bool init_done = false;
    if(init_done) // In the next call, it will be true, if it succeeds
        return 0;

    /// Initializes queues
    static FILA2 _able, _blocked;
    able = &_able;
    blocked = &_blocked;
    if(CreateFila2(able) != 0) return -1;
    if(CreateFila2(blocked) != 0) return -1;

    /// Initializes main TCB
    static TCB_t main_tcb = {
        .tid = 0,
        .state = PROCST_EXEC,
        .prio = NUM_PRIO-1, // Main thread shall have the lowest priority
    };
    executing = &main_tcb;

    init_done = true;
    return 0;
}


/**************************************
 *  Internal functions (not exposed)  *
 **************************************/

int ccreate (void* (*start)(void*), void *arg, int prio)
{
    if(init_cthread() != 0) return -1;

    (void)start; (void)arg; (void)prio;
	return -1;
}

int csetprio(int tid, int prio)
{
    if(init_cthread() != 0) return -1;

    (void)tid; (void)prio;
	return -1;
}

int cyield(void)
{
    if(init_cthread() != 0) return -1;

	return -1;
}

int cjoin(int tid)
{
    if(init_cthread() != 0) return -1;

    (void)tid;
	return -1;
}

int csem_init(csem_t *sem, int count)
{
    if(init_cthread() != 0) return -1;

    (void)sem; (void)count;
	return -1;
}

int cwait(csem_t *sem)
{
    if(init_cthread() != 0) return -1;

    (void)sem;
	return -1;
}

int csignal(csem_t *sem)
{
    if(init_cthread() != 0) return -1;

    (void)sem;
	return -1;
}

int cidentify (char *name, int size)
{
    char comp[][128] =
    {
        "00243451 Giovane Fonseca",
        "00274744 Matheus F. Kovaleski",
        "00231592 Yuri Jaschek",
    };

    for(int i=0; i<ARRAY_SIZE(comp); i++)
    {
        int n = snprintf(name, size, "%s\n", comp[i]);
        name += n;
        size -= n;
    }
	return 0;
}

