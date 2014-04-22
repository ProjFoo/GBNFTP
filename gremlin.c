#include "gremlin.h"
#include "debug.h"

#include <stdio.h>

int gremlin(char ** message, float damaged, float lost, float delay, int delaytime, unsigned int r) {

#ifdef DEBUG
    printf("Message: %s\n"
           "Damage %: %i\n"
           "Loss %: %i\n"
           "Delay %: %i\n"
           "Delay Time: %i ms"
           , message, damage, lost, delay, delaytime);
#endif

	int returnNum;
	int sizeOfPacket = strlen(*packet);
	int s = r % 100;
	int p_dam = damaged * 100,
		p_lost = lost * 100,
		p_delay = delay * 100;


	if (r <= p_dam)
	{

		damage(packet);

		returnNum = 1;
	}
	else if (r <= p_dam + p_lost)
	{

		returnNum = 0;

	}
	else if (r <= p_dam + p_lost + p_delay)
	{
	    delay(message, delaytime);
	    returnNum = 2;
	}
	else {
		returnNum = 1;
	}

	return returnNum;
}

char * damage(char **p)
{

	const int ONE = 7,
			TWO = 2,
			THREE = 1;

	char *msg = (*p);
	char corruptor = '0';
	int r = rand() % 10;



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
