#include "checksum.h"
#include "debug.h"

#include <stdio.h>

char* checksum(char* message)
{
    char mysum[16] = {'\0'},
    	 currentSubString[17],
    	 checksum[17];

    checksum[17] = '\0';
    currentSubString[17] = '\0';
	int i = 0;
	for( i ; i <= strlen(message); i = i + 16)
	{
		if( i + 16 > 494 ) {
			strcpy(currentSubString, message[i], 494 - i );
			int k = 494 - i;
			for( k; k < 16; k++ ){
				currentSubString[k] = '\0';
			}
		}
		strcpy(currentSubString, message[i], 16)
		mysum = mysum + currentSubString;
	}
	
	return mysum;
}


