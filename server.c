#include "packet.h"
#include "gremlin.h"
#include "checksum.h"
#include "debug.h"

#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    char fileName[256];
    FILE *fp;

    while( listen(fileName) ){}
    
#ifdef DEBUG
    printf("File Name: %s\n", fileName);
#endif
    //fp = fopen(fileName, "r");

    return 1;
}

int listen(char* fileName){
   printf("Listen is not yet implemented.\n"); 
   strcpy(fileName, "test.txt");
   return 0;
}
