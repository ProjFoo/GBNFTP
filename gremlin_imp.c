//example gremling implementation.
#include <sys/time.h>
#include "tqueue.h"
#include "pqueue.h"

struct timeval tv_init, tv_now;
struct pqueue my_pqueue;
struct tqueue my_tqueue;
unsigned long int time_then, time_now;

while(whatever) //main sending loop
{

    {   //this can literally be in any spot of the main loop. start or end doesn't matter.
        gettimeofday(tv_now);
        time_now = tv_now.tv_sec * 1000000 + tv_now.tv_usec;
        time_then = t_dequeue(my_tqueue);
        if( time_now >= time_then + delaytime * 1000 ) //delaytime is in msec, time_then usec
        {
            char* latepacket = p_dequeue(my_pqueue);
            send(packet);
        }
    }

    {   //gremlin integration. Should go where main send is.
        int gremlinSuccess = gremlin(message, damaged, lost, delay, randomSeed);

        switch(gremlinSuccess){
            case PACKET_LOST:
                //do nothing; packet is gone
                break;
            case PACKET_PASSED:
                //send packet here
                send(packet);
                break;
            case PACKET_DELAYED:
                gettimeofday(tv_init, NULL);
                time_now = tv_init.tv_sec * 1000000 + tv_init.tv_usec;
                p_enqueue(my_pqueue, packet); //packet should be char pointer (array) to 513-char string
                t_enqueue(my_tqueue, time_now);
    }
}
