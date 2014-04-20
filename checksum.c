#include "checksum.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

unsigned char* checksum(char* message, unsigned char* myChecksum)
{
    MD5_CTX mdContext;


    MD5_Init (&mdContext);
    MD5_Update(&mdContext, message, strlen(message));
    MD5_Final(myChecksum, &mdContext);

#ifdef DEBUG
    printf("Checksum is: %s\n", myChecksum);
#endif

	
	return myChecksum;
}


