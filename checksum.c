#include "checksum.h"
#include "debug.h"

#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

char* checksum(char* message, char* myChecksum)
{
    MD5_CTX mdContext;


    MD5_Init (&mdContext);
    MD5_Update(&mdContext, message, strlen(message));
    MD5_Final(myChecksum, &mdContext);


	//int i = 0;
//	for( i ; i <= strlen(message); i = i + 16)
//	{
//		if( i + 16 > strlen(message) ) {
//			memcpy(currentSubString, &message[i], strlen( message ) - i );
//			int k = strlen( message ) - i;
//			for( k; k < 16; k++ ){
//				currentSubString[k] = '\0';
//			}
//		}
//        else{
//		memcpy(currentSubString, &message[i], 16);
//        }
//
//        mysum = (int)mysum + (int)currentSubString;
#ifdef DEBUG
    printf("Checksum is: %s\n", myChecksum);
#endif

	
	return myChecksum;
}


