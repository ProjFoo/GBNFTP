#define QUEUESIZE 513*80

typedef struct {
    char* q[QUEUESIZE + 1];
    int first;
    int last;
    int count;
} pqueue;
