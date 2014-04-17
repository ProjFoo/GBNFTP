/*
** talker.c -- a datagram "client" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define MAXBUFLEN 129
#define MESSAGESIZE 124
#define ACKNAK_INDEX 0
#define NAK 0
#define ACK 1
#define FIN 2

int transfer(char acknack, char seqNum, char *checksum, char* body, struct addrinfo *p, int sockfd, char *hostname);
char* makeChecksum(char* message);


/*
Call with: IP PORT MESSAGE
*/
int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    int rv;
    int numbytes;
	char acknack;
	char seqNum;
	char *checksum;
	char *command;
	char *body, *buf, *fileName;

	FILE *fr;
	checksum = malloc(2);
	acknack = '1';
	seqNum	= '1';
	checksum = "11";
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 20000;
	short isFirstRun = 0;



	puts("What do you want to do?");
	fileName = malloc(50);
	command = malloc(50);
	scanf("%s", command);
	while(strcmp(command, "PUT") != 0){
		puts("Command not support. ctrc-c to end or enter another command");
		scanf("%s", command);
	}

	puts("What file would you like to put?");
	scanf("%s", fileName);
	puts("");
	fr = fopen(fileName, "r");
	if (fr == NULL) perror ("Error opening file");

    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }	

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socket\n");
        return 2;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
	{
		perror("Error:");
	}
    strcat(command, ",");
    strcat(command, fileName);

    buf = malloc(strlen(command));
    strcpy(buf, command);
    printf("Buf before file name send: %s\n", buf);
    if ((numbytes = sendto(sockfd, buf, strlen(buf), 0,
    				 p->ai_addr, p->ai_addrlen)) == -1) {
    			perror("talker: sendto");
    }

    char message[MESSAGESIZE + 1];
    char getChar;

    //seqNum = malloc(2);
    //strcpy(seqNum, "1");
    //seqNum[1] = '\0';

	while(getChar != EOF){

		int i = 0;

		for(i;  i < MESSAGESIZE + 1; i++)
		 {
		    message[i] = '\0';
		 }

		int messageLength = 0;
		while( messageLength < MESSAGESIZE && (getChar = fgetc(fr)) != EOF )
		  {
			message[messageLength] = getChar;
			messageLength++;
		  }

		buf = malloc(messageLength);

		strcpy(buf, message);
		puts("===Message Start===");
		while (transfer(acknack, seqNum, checksum, message, p, sockfd, argv[1]) == NAK)
		{

			//Keep looping until ACK is received
		}
		puts("===End Message===\n\n");

		if( seqNum == '1' )
		{
			seqNum = '0';
		}
		else
		{
			seqNum = '1';
		}
	}

	buf = malloc(1);
    buf = "\0";

    puts("===Sending Final Message===");
    transfer(acknack, seqNum, checksum, buf, p, sockfd, argv[1]);
	puts("===End Transfer===\n\n");

	freeaddrinfo(servinfo);
	fclose(fr);
    close(sockfd);

    return 0;
}

/*
 * Sends in the order of:
 * acknack, seqNum, checksum, body
 */
int transfer(char acknack, char seqNum, char *checksum, char* body, struct addrinfo *p, int sockfd, char *hostname)
{
	char *buf;
	char *test;

	buf = malloc(4 + strlen(body));

	checksum = malloc(3);
	checksum = makeChecksum(body);
	checksum[2] = '\0';
	test = malloc(5);
	test[0] = acknack;
	test[1] = seqNum;
	test[2] = checksum[0];
	test[3] = checksum[1];
	test[4]	= '\0';

	strcat(buf, test);
	strcat(buf, body);
	int numBytes;

	struct sockaddr_storage their_addr;
		socklen_t addr_len;

    if ((numBytes = sendto(sockfd, buf, strlen(buf), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
    }



   	printf("talker: sent %d bytes to %s\n", numBytes, hostname);

    //Acknak stuff=================================
        char *incMessage;
        incMessage = malloc(128);
        addr_len = sizeof their_addr;

        puts("Waiting for response from server...");
    	if ((numBytes = recvfrom(sockfd, incMessage, MAXBUFLEN , 0,
    		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
    		perror("recvfrom");
    		puts("The packet appears to have been lost.");
    	}
    	int ackNak;

    	ackNak = atoi(&incMessage[ACKNAK_INDEX]);

    	if(ackNak == ACK){
    		char *msgABP;
    		msgABP = malloc(2);
    		memcpy(msgABP, incMessage + 1, 1);
    		msgABP[1] = '\0';

    		if(msgABP[0] == seqNum){
    			//Success
    		}
    		else{
    			ackNak = NAK;
    		}
    	}

    	printf("Acknak: %d\n", ackNak);
    	printf("Sequence number: %c\n", seqNum);

    	//==============================================



    return ackNak;
}

char* makeChecksum(char* message)
{
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
}



