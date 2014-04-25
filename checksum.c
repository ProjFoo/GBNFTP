#include "checksum.h"
#include "debug.h"


#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>


char* checksum(char* message, char* myChecksum)
{


	//if we're using a sixteen byte checksum, MD5 it.
#ifdef SIXTEEN_BYTE_CHECKSUM
    MD5_CTX mdContext;




    MD5_Init (&mdContext);
    MD5_Update(&mdContext, message, strlen(message));
    MD5_Final(myChecksum, &mdContext);
#else
    int sum = 0;
    char mysum[20];
    char *returned;
    char returnedChecksum[3];
    memset(returnedChecksum, 0, 3);


	int i = 0;
	for( i=0 ; i <= strlen(message); i++)
	{
		sum = sum + (int)message[i];
	}


    sprintf(mysum, "%d", sum);
    i = 0;


    returnedChecksum[0] = mysum[strlen(mysum) - 2];
    returnedChecksum[1] = mysum[strlen(mysum) - 1];

    returned = malloc(3);
    strcpy(returned, returnedChecksum);

#endif


#ifdef DEBUG
    printf("Checksum is: %s\n", returned);
#endif




	return returned;
}
