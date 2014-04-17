#include "checksum.h"
#include "debug.h"

#include <stdio.h>

char* checksum(char* message)
{
    printf("Checksum not yet implemented.");

    return "111";
    /*
    int sum = 0;
    char *mysum = malloc(20),
    	 *checksum = malloc(3);

    checksum[2] = '\0';
	int i = 0;
	for( i ; i <= strlen(message); i++)
	{
		sum = sum + (int)message[i];
	}


    char c[20];
    sprintf(mysum, "%d", sum);
    i = 0;

    checksum[0] = mysum[strlen(mysum) - 2];
    checksum[1] = mysum[strlen(mysum) - 1];
    checksum[2] = '\0';


    free(mysum);
	return checksum;
    */
}


