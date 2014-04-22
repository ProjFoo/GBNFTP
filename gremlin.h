#ifndef GREMLIN_H_INCLUDED
#define GREMLIN_H_INCLUDED

#define PACKET_LOST  0
#define PACKET_PASSED 1
#define PACKET_DELAYED 2

int gremlin(char ** message, float damaged, float lost, float delay, int delaytime, unsigned int r);
char * damage(char **p, unsigned int r);
#endif
