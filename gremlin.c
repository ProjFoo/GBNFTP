#include "gremlin.h"
#include "debug.h"

#include <stdio.h>

int gremlin(char* message, int damage, int lost, int delay, int delaytime) {

#ifdef DEBUG
    printf("Message: %s\n"
           "Damage %: %i\n"
           "Loss %: %i\n"
           "Delay %: %i\n"
           "Delay Time: %i ms"
           , message, damage, lost, delay, delaytime);
#endif
    printf("Gremlin functionality has yet to be implemented.\n");

    return 1;
}
