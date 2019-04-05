/******************************************************************************
 *  Instituto de Informática - Universidade Federal do Rio Grande do Sul      *
 *  INF01142 - Sistemas Operacionais I N                                      *
 *  Implementação própria da biblioteca de suporte (support.h)                *
 *                                                                            *
 *  As funções Random2(), startTimer() e stopTimer() não estão implementadas  *
 *                                                                            *
 ******************************************************************************/

#include "support.h"
#include <stdlib.h>
#include <time.h>

int CreateFila2(PFILA2 pFila)
{
    pFila->first = pFila->it = pFila->last = NULL;
    return 0;
}

int FirstFila2(PFILA2 pFila)
{
    pFila->it = pFila->first;
    if(!pFila->it)
        return -1;
    return 0;
}

int LastFila2(PFILA2 pFila)
{
    pFila->it = pFila->last;
    if(!pFila->it)
        return -1;
    return 0;
}

int NextFila2(PFILA2 pFila)
{
    if(!pFila->first)
        return -NXTFILA_VAZIA;
    if(!pFila->it)
        return -NXTFILA_ITERINVAL;
    pFila->it = pFila->it->next;
    if(!pFila->it)
        return -NXTFILA_ENDQUEUE;
    return 0;
}

void *GetAtIteratorFila2(PFILA2 pFila)
{
    if(!pFila->it)
        return NULL;
    return pFila->it->node;
}

void *GetAtNextIteratorFila2(PFILA2 pFila)
{
    if(!pFila->it || !pFila->it->next)
        return NULL;
    return pFila->it->next->node;
}

void *GetAtAntIteratorFila2(PFILA2 pFila)
{
    if(!pFila->it || !pFila->it->ant)
        return NULL;
    return pFila->it->ant->node;
}

int AppendFila2(PFILA2 pFila, void *content)
{
    PNODE2 node = malloc(sizeof(NODE2));
    if(!node)
        return -1;
    node->node = content;
    node->ant = pFila->last;
    node->next = NULL;
    if(pFila->first) // Queue is not empty, so there is a last element
        pFila->last->next = node;
    else // Queue is empty
        pFila->first = node;
    pFila->last = node;
    return 0;
}

int InsertAfterIteratorFila2(PFILA2 pFila, void *content)
{
    if(!pFila->first)
        return -INSITER_VAZIA;
    if(!pFila->it)
        return -INSITER_INVAL;
    PNODE2 data = malloc(sizeof(NODE2));
    if(!data)
        return -3;
    data->node = content;
    data->ant = pFila->it;
    data->next = pFila->it->next;
    if(pFila->it->next) // It is not the last one
        pFila->it->next->ant = data;
    else // It is the last one
        pFila->last = data;
    pFila->it->next = data;
    return 0;
}

int InsertBeforeIteratorFila2(PFILA2 pFila, void *content)
{
    if(!pFila->first)
        return -INSITER_VAZIA;
    if(!pFila->it)
        return -INSITER_INVAL;
    PNODE2 data = malloc(sizeof(NODE2));
    if(!data)
        return -3;
    data->node = content;
    data->next = pFila->it;
    data->ant = pFila->it->ant;
    if(pFila->it->ant) // It is not the first one
        pFila->it->ant->next = data;
    else // It is the first one
        pFila->first = data;
    pFila->it->ant = data;
    return 0;
}

int DeleteAtIteratorFila2(PFILA2 pFila)
{
    PNODE2 it = pFila->it;
    if(!pFila->first)
        return -DELITER_VAZIA;
    if(!pFila->it)
        return -DELITER_INVAL;

    if(pFila->it->next)
        pFila->it->next->ant = pFila->it->ant;
    else
        pFila->last = pFila->it->ant;

    if(pFila->it->ant)
        pFila->it->ant->next = pFila->it->next;
    else
        pFila->first = pFila->it->next;

    pFila->it = pFila->it->next;
    free(it);
    return 0;
}

unsigned int Random2(void)
{
    return 0;
}

void startTimer(void)
{

}

unsigned int stopTimer(void)
{
    return 0;
}


