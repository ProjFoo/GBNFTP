#include "gremlin.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

int gremlin(char ** message, float damaged, float lost, float delay, int delaytime, unsigned int r) {



#ifdef DEBUG
    printf("Message: %s\n"
           "Damage %: %i\n"
           "Loss %: %i\n"
           "Delay %: %i\n"
           "Delay Time: %i ms"
           , message, damaged, lost, delay, delaytime);
#endif

	int returnNum;
	int sizeOfPacket = strlen(*message);
	int s = r % 100;
	int p_dam = damaged * 100,
		p_lost = lost * 100,
		p_delay = delay * 100;


	if (r <= p_dam)
	{
		puts("Damaged the file");
		damage(message, r);

		returnNum = PACKET_PASSED;
	}
	else if (r <= p_dam + p_lost)
	{

		returnNum = PACKET_LOST;

	}
	else if (r <= p_dam + p_lost + p_delay)
	{
	    returnNum = PACKET_DELAYED;
	}
	else {
		returnNum = PACKET_PASSED;
	}

	return returnNum;
}

char * damage(char **p, unsigned int r)
{

	srand(r);
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
