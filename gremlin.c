#include "gremlin.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

int gremlin(char ** message, float damaged, float lost, float delay, int delaytime, int r) {



#ifdef DEBUG
	/*
    printf("Message: %s\n"
           "Damage %: %i\n"
           "Loss %: %i\n"
           "Delay %: %i\n"
           "Delay Time: %i ms"
           , message, damaged, lost, delay, delaytime);
     */
#endif

	int returnNum;
	int sizeOfPacket = strlen(*message);
	//int s = r % 100;
	int p_dam = damaged * 100,
		p_lost = lost * 100,
		p_delay = delay * 100;
//printf("CurrentProbability is %d\n", r);

	if (r <= p_dam)
	{

		damage(message, r);
	//	puts("Packet Damaged");
		returnNum = PACKET_PASSED;
	}
	else if (r <= p_dam + p_lost)
	{
		//puts("Packet Lost");
		returnNum = PACKET_LOST;

	}
	else if (r <= p_dam + p_lost + p_delay)
	{
		//puts("Packet Delayed");
	    returnNum = PACKET_DELAYED;
	}
	else {
		//puts("Packet is Fine");
		returnNum = PACKET_PASSED;
	}

	return returnNum;
}

char * damage(char **p, unsigned int r)
{


	const int ONE = 7,
			TWO = 2,
			THREE = 1;

	char *msg = (*p);
	char corruptor = '0';
	r = rand() % 10;



	if (r < ONE )
	{

		int s = rand() % sizeof(*p);
		msg[s] = corruptor;

	}

	else if(r < (ONE + TWO) )
	{

		int s = rand() % 124;
		msg[s] = corruptor;
		s = rand() % 124;
		while (msg[s] == corruptor)
			{
			s = rand() % 124;
			msg[s] = corruptor;
			s = rand() % 124;
			}
	}
	else if (r < ONE + TWO + THREE)
	{

		int s = rand() % 124;
		msg[s] = corruptor;

		while (msg[s] == corruptor)
			{
				s = rand() % 124;
				msg[s] = corruptor;
				s = rand() % 124;
			}
		while (msg[s] == corruptor)
			{
				s = rand() % 124;
				msg[s] = corruptor;
				s = rand() % 124;
			}
	}

	else
	{

	}
	return *p;
}
