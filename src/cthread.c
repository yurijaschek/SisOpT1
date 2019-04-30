/*****************************************************************************
 *  Instituto de Informática - Universidade Federal do Rio Grande do Sul     *
 *  INF01142 - Sistemas Operacionais I N                                     *
 *  Trabalho Prático 1 - compact thread (cthread) 2019/1                     *
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
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(x) (int)(sizeof(x) / sizeof((x)[0]))

#define STACK_SIZE  8192    // Stack size for each thread
#define NUM_PRIO    3       // Number of possible priorities (3 for 2019/1), from 0 to NUM_PRIO
#define PREEMPTIVE  0       // If the scheduler is preemptive or not (0 for 2019/1)


/**********************
 *  Global variables  *
 **********************/

static int tid_counter = 1; // Incremental counter for thread ID (tid) assignments. Main will be 0

static PFILA2 able;         // Queue of threads able to be executed (selected by the scheduler)
static PFILA2 blocked;      // Queue of the blocked threads, awaiting an unblocking event
static PFILA2 finished;     // Queue of the threads that have finished execution
static TCB_t *executing;    // Thread control block (TCB) of the thread that is currently using the CPU

static ucontext_t context_end;  // Context where processes go after they finish execution


/**************************************
 *  Internal functions (not exposed)  *
 **************************************/

/** TCB and context management functions */

/*-----------------------------------------------------------------------------
Funct:  Frees the TCB passed as parameter, including the thread's stack.
Input:  tcb -> Pointer to the TCB to be freed.
-----------------------------------------------------------------------------*/
static void free_tcb(TCB_t *tcb)
{
    if(tcb)
        free(tcb->context.uc_stack.ss_sp);

    free(tcb);
}


/*-----------------------------------------------------------------------------
Funct:  Creates an execution context for the function and arguments passed.
Input:  uc      -> Pointer where the context will be created.
        funct   -> Function to be executed in the context.
        arg     -> Argument to the function.
Return: If the function succeeds, 0 will be returned. Otherwise -1.
-----------------------------------------------------------------------------*/
static int create_context(ucontext_t *uc, void *funct, void *arg)
{
    if(getcontext(uc) != 0) // Gets a context "model"
        return -1;

    stack_t *sp = malloc(STACK_SIZE); // Stack area for the execution context
    if(!sp)
        return -1;

    uc->uc_stack.ss_sp = sp; // Sets stack pointer
    uc->uc_stack.ss_size = STACK_SIZE; // Sets stack size
    uc->uc_link = &context_end; // Sets the context to go after execution

    makecontext(uc, funct, 1, arg); // "Makes everything work" (can't fail)
    return 0;
}


/*-----------------------------------------------------------------------------
Funct:  Creates a thread having the given priority. Also creates the thread's
        context with the given function and argument.
Input:  funct   -> Function to be executed in the context.
        arg     -> Argument to the function.
        prio    -> Priority of the thread.
Return: If the function succeeds, return a pointer to its TCB, else NULL.
-----------------------------------------------------------------------------*/
static TCB_t *create_thread(void *funct, void *arg, int prio)
{
    TCB_t *tcb = malloc(sizeof(TCB_t)); // Allocates TCB
    if(!tcb)
        return NULL;

    memset(tcb, 0, sizeof(TCB_t)); // Clears the structure

    if(create_context(&tcb->context, funct, arg) != 0)
    {
        free_tcb(tcb);
        return NULL;
    }

    tcb->tid = tid_counter++; // The counter is upwards only
    tcb->state = PROCST_APTO; // Makes it able to execute
    tcb->prio = prio; // Sets the given priority
    return tcb;
}


/** Queue helper functions */

/*-----------------------------------------------------------------------------
Funct:  Finds a TCB by its tid in the given queue, removing it, if asked.
Input:  tid -> Thread id to be found.
        q   -> Tid queue to be searched.
        rem -> If the TCB is to be removed or not from the queue, if found.
Return: If the tid's TCB is in the queue, return a pointer to it, else NULL.
-----------------------------------------------------------------------------*/
static TCB_t *find_by_tid_queue(int tid, PFILA2 q, bool rem)
{
    if(FirstFila2(q) != 0) // If it's empty
        return NULL;

    do // Till there's no next element
    {
        TCB_t *tcb = (TCB_t*)GetAtIteratorFila2(q); // Retrieve current element (at iterator)
        if(tcb && tcb->tid == tid) // Found it
        {
            if(rem) // Delete if requested
                DeleteAtIteratorFila2(q);
            return tcb;
        }

    } while(NextFila2(q) == 0);

    return NULL; // Not in the queue
}


/*-----------------------------------------------------------------------------
Funct:  Finds a TCB by its tid in all queues and executing.
Input:  tid -> Thread id to be found.
Return: If the tid's TCB exists, return a pointer to it, else NULL.
-----------------------------------------------------------------------------*/
static TCB_t *find_by_tid(int tid)
{
    if(executing && executing->tid == tid) // Thread is executing
        return executing;

    PFILA2 queues[] = {able, blocked, finished}; // Queues to be searched
    for(int i=0; i<ARRAY_SIZE(queues); i++)
    {
        TCB_t *tcb = find_by_tid_queue(tid, queues[i], false);
        if(tcb)
            return tcb;
    }

    return NULL; // Not in any queues
}


/*-----------------------------------------------------------------------------
Funct:  Finds the highest-priority TCB in the queue, removing it, if asked.
Input:  q   -> Tid queue to be searched.
        rem -> If the TCB is to be removed or not from the queue, if found.
Return: Pointer to the first TCB of highest priority, or NULL, if not found.
-----------------------------------------------------------------------------*/
static TCB_t *find_highest(PFILA2 q, bool rem)
{
    if(FirstFila2(q) != 0) // If it's empty
        return NULL;

    TCB_t *ans = (TCB_t*)GetAtIteratorFila2(q); // Set highest as first
    while(NextFila2(q) == 0) // While there's a next element in the queue
    {
        TCB_t *tcb = (TCB_t*)GetAtIteratorFila2(q);

        if(tcb->prio < ans->prio) // And not "<=" so we keep the first highest found
            ans = tcb;
    }

    if(rem)
        find_by_tid_queue(ans->tid, q, true); // To remove only

    return ans;
}


/** Scheduler functions */

/*-----------------------------------------------------------------------------
Funct:  Choose a thread and execute it. In the unlikely event that there is no
        no thread to execute (will happen to deadlocks), exit application.
-----------------------------------------------------------------------------*/
static void scheduler(void)
{
    ucontext_t *ctx = &executing->context; // To know where to swap at the end of scheduler

    executing = find_highest(able, true); // Choose and delete from able queue
    if(!executing) // No threads to execute
    {
        printf("Deadlock: No threads to schedule! Exiting now...\n");
        exit(1);
    }

    executing->state = PROCST_EXEC;
    swapcontext(ctx, &executing->context);
}


/*-----------------------------------------------------------------------------
Funct:  Stop execution of the current thread, putting it in the appropriate
        queue and "jump" to scheduler.
Input:  state   -> Defines the queue (state) to put the current thread.
-----------------------------------------------------------------------------*/
static void stop_execution(int state)
{
    executing->state = state;
    switch(state)
    {
      case PROCST_APTO:
        AppendFila2(able, executing); break;
      case PROCST_BLOQ:
        AppendFila2(blocked, executing); break;
      case PROCST_TERMINO:
        AppendFila2(finished, executing); break;
    }

    scheduler();
}


/*-----------------------------------------------------------------------------
Funct:  A thread that has finished execution goes to this function, that
        performs the necessary actions after a thread's end, such as putting in
        able a thread that was waiting for this one to finish execution.
-----------------------------------------------------------------------------*/
static void thread_end(void)
{
    TCB_t *tcb = executing->wait;
    if(tcb) // There is a thread waiting for the current one
    {
        find_by_tid_queue(tcb->tid, blocked, true); // Remove from blocked
        AppendFila2(able, tcb); // Put it in able
        executing->wait = NULL;
    }

    stop_execution(PROCST_TERMINO); // Will put this in finished and schedule
}


/** Initialization function */

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
    static FILA2 _able, _blocked, _finished;
    able = &_able;
    blocked = &_blocked;
    finished = &_finished;
    if(CreateFila2(able) != 0) return -1;
    if(CreateFila2(blocked) != 0) return -1;
    if(CreateFila2(finished) != 0) return -1;

    /// Creates context_end
    if(create_context(&context_end, thread_end, NULL) != 0)
        return -1;

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


/**********************************
 *  API open functions (exposed)  *
 **********************************/

int ccreate (void* (*start)(void*), void *arg, int prio)
{
    if(init_cthread() != 0) return -1;

    if(prio < 0 || prio >= NUM_PRIO)
        return -1;

    TCB_t *tcb = create_thread(start, arg, prio);
    if(!tcb || AppendFila2(able, tcb) != 0)
    {
        free_tcb(tcb);
        return -1;
    }
#if PREEMPTIVE
    if(prio < executing->prio)
        stop_execution(PROCST_APTO);
#endif
	return tcb->tid;
}


int csetprio(int tid, int prio)
{
    (void)tid; // This value is not used in 2019/1
    if(init_cthread() != 0) return -1;

    if(prio < 0 || prio >= NUM_PRIO)
        return -1;
    executing->prio = prio;
#if PREEMPTIVE
    TCB_t *tcb = find_highest(able, false);
    if(tcb && tcb->prio < prio) // There is a thread with higher priority
        stop_execution(PROCST_APTO);
#endif
	return 0;
}


int cyield(void)
{
    if(init_cthread() != 0) return -1;

    stop_execution(PROCST_APTO);
	return 0;
}


int cjoin(int tid)
{
    if(init_cthread() != 0) return -1;
    TCB_t *tcb = find_by_tid(tid);
    if(!tcb || tcb->state == PROCST_TERMINO || tcb->wait) // Doesn't exist, has
        return -1; // finished or another one is waiting for this thread

    tcb->wait = executing; // So we remember to unblock the current one after that ends
    stop_execution(PROCST_BLOQ);
    return 0;
}


int csem_init(csem_t *sem, int count)
{
    if(init_cthread() != 0) return -1;

    memset(sem, 0, sizeof(csem_t));

    sem->fila = malloc(sizeof(FILA2));
    if(!sem->fila)
        return -1;

    if(CreateFila2(sem->fila) != 0)
        return -1;

    sem->count = count;
	return 0;
}


int cwait(csem_t *sem)
{
    if(init_cthread() != 0) return -1;

    if(--sem->count < 0)
    {
        AppendFila2(sem->fila, executing);
        stop_execution(PROCST_BLOQ);
    }
	return 0;
}


int csignal(csem_t *sem)
{
    if(init_cthread() != 0) return -1;

    sem->count++;

    TCB_t *tcb = find_highest(sem->fila, true);
    if(tcb)
    {
        find_by_tid_queue(tcb->tid, blocked, true); // Remove from blocked
        AppendFila2(able, tcb);
#if PREEMPTIVE
        if(tcb->prio < executing->prio)
            stop_execution(PROCST_APTO);
#endif
    }
	return 0;
}


int cidentify (char *name, int size)
{
    char comp[][128] =
    {
        "00243451 Giovane Fonseca",
        "00274744 Matheus F. Kovaleski",
        "00231592 Yuri Jaschek",
    };

    *name++ = '\n'; size--;
    for(int i=0; i<ARRAY_SIZE(comp); i++)
    {
        int n = snprintf(name, size, "%s\n", comp[i]);
        name += n;
        size -= n;
    }
	return 0;
}

