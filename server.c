/*
 ** listener.c -- a datagram sockets "server" demo
 */

#include "server.h"
#define PACKETSIZE 513
#define WINDOWSIZE 16
#define MODULUS 32

char currPacket[PACKETSIZE];
char packetBuffer[MODULUS][PACKETSIZE] = {'\0'};


int isSent = 1;
int seqNum = MODULUS;
int packetsSent = -1;
int base = 0;
int expectedSeqNum = 1;
int viewFirstTen = 0;

void buildPacket(char *seqNum, char *checkSum, char *acknak, char *message);
void makeSeqNum(int seqNum, char* seqStr);
void getMessage(char* message, FILE *fr, char *getChar);

int main(void) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;

	char buf[MAXBUFLEN];
	struct sockaddr_storage their_addr;
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
	for (p = servinfo; p != NULL ; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
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

	if (p == NULL ) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);
	while (1) {
		int i;
		char *testGET;
		char *fileName;
		for (i = 0; i < MAXBUFLEN; i++) {
			buf[i] = '\0';
		}

		printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN, 0,
				(struct sockaddr *) &their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		printf("listener: packet contains \"%s\"\n", buf);

		if (strcmp("GET", checkForGet(buf)) == 0) {
			puts("Got GET successfully.");

			if (checkIfFileExist(buf, &fileName) == 1) {
				replyWithValidFile(p, sockfd, their_addr, addr_len);
				puts("Valid file message sent.");
				sendFile(fileName, p, sockfd, their_addr, addr_len);

			} else {
				replyWithInvalidFile(p, sockfd, their_addr, addr_len);
				puts("Invalid file response sent.\n\n");
			}

		}

	}
	close(sockfd);

	return 0;
}

void sendFile(char *fileName, struct addrinfo *p, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len) {


	int N = WINDOWSIZE,
		requestNumber = -1,
		sequenceNumber = 0,
		sequenceBase = 0,
		sequenceMax = N - 1;

	FILE *fr;
	fr = fopen(fileName, "r");
	char message[MESSAGESIZE + 1];
	char getChar;
	int numBytes;
	char *acknak, *checkSum;
	acknak = "1";
	checkSum = "11";
	char seqNumOut[2];

	struct pollfd *ufds;
	ufds->fd = sockfd;

	char incSeqNum[2];
	char incAckNak;
	char incmessage[6];
	memset(incmessage, 0, sizeof(incmessage));

	while (getChar != EOF) {  //while there is file to be sent

		while(poll(ufds, 1, 1)) {
			recvfrom(sockfd, incmessage, 5, 0, (struct sockaddr *) &their_addr, &addr_len);
			incAckNak = incmessage[4];
			incSeqNum[0] = incmessage[0];
			incSeqNum[1] = incmessage[1];
			requestNumber = atoi(incSeqNum);
			if( requestNumber > sequenceBase ){
				sequenceMax = (sequenceMax + (requestNumber - sequenceBase)) % 32;
				sequenceBase = requestNumber;
			} else if ( sequenceMax < N && sequenceMax >= requestNumber  ) {
				sequenceMax = (sequenceMax + (requestNumber - sequenceBase));
				sequenceBase = requestNumber;
			}
		}

		for(sequenceNumber = sequenceBase; sequenceNumber - sequenceBase < N; sequenceNumber++) {
			if( strlen(packetBuffer[sequenceNumber]) == 0 ) {
				makeSeqNum(sequenceNumber, seqNumOut);
				getMessage(message, fr, &getChar);
				buildPacket(seqNumOut, checkSum, acknak, message);
				strcpy(packetBuffer[sequenceNumber], currPacket);
			}
			sendto(sockfd, packetBuffer[sequenceNumber], strlen(packetBuffer[sequenceNumber]),
					0, (struct sockaddr *) &their_addr, addr_len);
			if(poll(ufds, 1, 1)){
				recvfrom(sockfd, incmessage, 5, 0, (struct sockaddr *) &their_addr, &addr_len);
				incAckNak = incmessage[4];
				incSeqNum[0] = incmessage[0];
				incSeqNum[1] = incmessage[1];
				requestNumber = atoi(incSeqNum);
				if( requestNumber > sequenceBase ){
					sequenceMax = (sequenceMax + (requestNumber - sequenceBase)) % 32;
					sequenceBase = requestNumber;
				} else if ( sequenceMax < N && sequenceMax >= requestNumber  ) {
					sequenceMax = (sequenceMax + (requestNumber - sequenceBase));
					sequenceBase = requestNumber;
				}
			}
		}
	}

	puts("Running memset on message");
	memset(message, 0, MESSAGESIZE + 1);
	puts("Setting Acknak");
	acknak = "2";
	puts("Building final packet");
	buildPacket(seqNumOut, checkSum, acknak, message);

	printf("FinalCurrentPacket = %s\n", currPacket);

	puts("Final packet sent...Waiting for response...");

	if ((numBytes = sendto(sockfd, currPacket, strlen(currPacket), 0,
			(struct sockaddr *) &their_addr, addr_len)) == -1) {
		perror("talker: sendto");
	}

	puts("File transfer complete!");
	fclose(fr);

}

void replyWithInvalidFile(struct addrinfo *p, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len) {

	int numBytes;
	char message[8] = "invalid";

	if ((numBytes = sendto(sockfd, message, strlen(message), 0,
			(struct sockaddr *) &their_addr, addr_len)) == -1) {
		perror("talker: sendto");
	}
}

void replyWithValidFile(struct addrinfo *p, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len) {

	int numBytes;
	char message[8] = "valid";

	if ((numBytes = sendto(sockfd, message, strlen(message), 0,
			(struct sockaddr *) &their_addr, addr_len)) == -1) {
		perror("talker: sendto");
	}
}

void buildPacket(char *seqNum, char *checkSum, char *acknak, char *message) {
	//puts("\nZeroing out current packet before building.");
	memset(currPacket, 0, sizeof(currPacket));
	//puts("Copying sequence number into packet");
	//printf("SeqNumOutBuild = %s\n", seqNum);
	strcpy(currPacket, seqNum);
	//puts("Concatenating Checksum into packet");
	strcat(currPacket, checkSum);
	//puts("Concatenating acknak into packet");
	strcat(currPacket, acknak);
	//puts("Concatenating Message into packet");
	strcat(currPacket, message);
	//printf("Message = %s\n", message);
	//puts("Packet built and ready for transfer!\n");
}

int checkIfFileExist(char *buf, char **fileName) {
	char *splitString, *tempFileName;
	int valid = 1;
	FILE *fr;

	splitString = strsep(&buf, ",");

	fr = fopen(&buf[0], "r");
	if (fr == NULL ) {
		valid = 0;
		puts("Invalid File.");
	} else {
		fclose(fr);
	}

	*fileName = &buf[0];

	return valid;
}

char* checkForGet(char *packetIn) {
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

void makeSeqNum(int seqNum, char* seqStr){
	int seqNumMod = seqNum % MODULUS;
	//puts("Casting sequence# to char array");
	sprintf(seqStr, "%ld", seqNumMod);
	if (seqNumMod < 10) {
		seqStr[0] = '0';
		seqStr[1] = (char) (((int) '0') + seqNumMod);
	}
}

void getMessage(char* message, FILE *fr, char *getChar){
	memset(message, 0, MESSAGESIZE + 1);
	int messageLength = 0;

	while (messageLength < MESSAGESIZE) {
		if( ( getChar[0] = fgetc(fr) ) != EOF ) {
			message[messageLength] = getChar[0];
		}
		else{
			message[messageLength] = '\0';
		}
		messageLength++;
	}
}
