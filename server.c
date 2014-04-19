/*
** listener.c -- a datagram sockets "server" demo
*/

#include "server.h"
#define PACKETSIZE 513

char currPacket[PACKETSIZE];

void buildPacket(char *seqNum, char *checkSum, char *acknak, char *message);


int main(void)
{
	int sockfd;
	    struct addrinfo hints, *servinfo, *p;
	    int rv;
	    int numbytes;
	    struct sockaddr_storage their_addr;
	    char buf[MAXBUFLEN];
	    socklen_t addr_len;
	    char s[INET6_ADDRSTRLEN];
	    memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	    hints.ai_socktype = SOCK_DGRAM;
	    hints.ai_flags = AI_PASSIVE; // use my IP

	    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	        return 1;
	    }

	    // loop through all the results and bind to the first we can
	    for(p = servinfo; p != NULL; p = p->ai_next) {
	        if ((sockfd = socket(p->ai_family, p->ai_socktype,
	                p->ai_protocol)) == -1) {
	            perror("listener: socket");
	            continue;
	        }

	        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	            close(sockfd);
	            perror("listener: bind");
	            continue;
	        }

	        break;
	    }

	    if (p == NULL) {
	        fprintf(stderr, "listener: failed to bind socket\n");
	        return 2;
	    }

	    freeaddrinfo(servinfo);
	    while(1){
	    	int i;
	    	char *testGET;
	    	char *fileName;
			for(i = 0; i < MAXBUFLEN; i++){
				buf[i] = '\0';
			}

			printf("listener: waiting to recvfrom...\n");

			addr_len = sizeof their_addr;
			if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0,
				(struct sockaddr *)&their_addr, &addr_len)) == -1) {
				perror("recvfrom");
				exit(1);
			}


			printf("listener: packet contains \"%s\"\n", buf);
			if(strcmp("GET", checkForGet(buf)) == 0){
				puts("Got GET successfully.");

				if(checkIfFileExist(buf, &fileName) == 1){

					//sendFile(fileName)

				}
				else{
					replyWithInvalidFile(p, sockfd, their_addr, addr_len);
					puts("Invalid file response sent.");
				}

			}

	    }
	    close(sockfd);

	    return 0;
}

void replyWithInvalidFile(struct addrinfo *p, int sockfd, struct sockaddr_storage their_addr, socklen_t addr_len){

	int numBytes;
	char message[8] = "invalid";

	if ((numBytes = sendto(sockfd, message, strlen(message), 0,
			(struct sockaddr *)&their_addr, addr_len)) == -1) {
				perror("talker: sendto");
	}
}

int checkIfFileExist(char *buf, char **fileName){
	char *splitString, *tempFileName;
	int valid = 1;
	FILE *fr;


	splitString = strsep(&buf, ",");



	fr = fopen(&buf[0], "r");
	if(fr == NULL){
		valid = 0;
		puts("Invalid File.");
	}
	else{
		fclose(fr);
	}

	return valid;
}

char* checkForGet(char *packetIn){
	char *splitString;
	char *tempPacket;
	char *cmp;
	int valid = 0;
	char COMMAND[3] = "GET";

	tempPacket = malloc(strlen(packetIn));
    strcpy(tempPacket, packetIn);



	splitString = strsep(&tempPacket, ",");

	return splitString;
}

void buildPacket(char *seqNum, char *checkSum, char *acknak, char *message)
{
	puts("Zeroing out current packet before building.");
	memset(currPacket, 0, sizeof(currPacket));
	puts("Copying sequence number into packet");
	strcpy(currPacket, seqNum);
	puts("Concatenating Checksum into packet");
	strcat(currPacket, checkSum);
	puts("Concatenating acknak into packet");
	strcat(currPacket, acknak);
	puts("Concatenating Message into packet");
	strcat(currPacket, message);
	puts("Packet built and ready for deployment SIR!");
}
