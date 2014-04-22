#include "checksum.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

unsigned char* checksum(char* message, unsigned char* myChecksum)
{
	
	//if we're using a sixteen byte checksum, MD5 it.
#ifdef SIXTEEN_BYTE_CHECKSUM
    MD5_CTX mdContext;


    MD5_Init (&mdContext);
    MD5_Update(&mdContext, message, strlen(message));
    MD5_Final(myChecksum, &mdContext);
#else
    int sum = 0;
    char *mysum = malloc(20),

    myChecksum[2] = '\0';
	int i = 0;
	for( i ; i <= strlen(message); i++)
	{
		sum = sum + (int)message[i];
	}

    sprintf(mysum, "%d", sum);
    i = 0;

    myChecksum[0] = mysum[strlen(mysum) - 2];
    myChecksum[1] = mysum[strlen(mysum) - 1];
    myChecksum[2] = '\0';


    free(mysum);
#endif

#ifdef DEBUG
    printf("Checksum is: %s\n", myChecksum);
#endif

	
	return myChecksum;
}


