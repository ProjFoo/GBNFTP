#include "tqueue.h"
#include <stdio.h>

init_tqueue(tqueue *p)
{
    p->first = 0;
    p->last = 80 -1;
    p->count = 0;
}

t_enqueue( tqueue *p, unsigned long int x )
{
    if ( p->count >= 80 )
    {
        printf("Warning: queue overflow");
    }
    else
    {
        p->last = (p->last + 1) % 80;
        p->q[ p->last ] = x;
        p->count = p->count + 1;
    }
}

unsigned long int t_dequeue(tqueue *p)
{
    unsigned long int x;

    if( (p->count) <= 0 )
    {
        printf("Warning: empty queue dequeue.\n");
    }
    else
    {
        x = p->q[p->first];
        p->first = (p->first + 1) % QUEUESIZE;
        p->count = p->count - 1;
    }

    return x;
}

int t_empty(queue *p){
    if (p->count <= 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
