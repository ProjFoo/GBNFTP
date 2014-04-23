#define QUEUESIZE 80

typedef struct {
    unsigned long int q[QUEUESIZE + 1];
    int first;
    int last;
    int count;
} tqueue;
