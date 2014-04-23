#include "pqueue.h"
#include <stdio.h>
#include <string.h>

init_pqueue(pqueue *p)
{
    p->first = 0;
    p->last = 80 -1;
    p->count = 0;
}

p_enqueue( pqueue *p, char *r )
{
    if ( p->count >= 80 )
    {
        printf("Warning: queue overflow");
    }
    else
    {
        p->last = (p->last + 1) % 80;

        char* c = p->q[p->last * 513];

        strcpy(c, r);
        p->count = p->count + 1;
    }
}

char* p_dequeue(pqueue *p)
{
    char *r;

    if( (p->count) <= 0 )
    {
        printf("Warning: empty queue dequeue.\n");
    }
    else
    {
        r = p->q[p->first * 513];
        p->first = (p->first + 1) % QUEUESIZE;
        p->count = p->count - 1;
    }

    return r;
}

int p_empty(queue *p){
    if (p->count <= 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
