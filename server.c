/*
 ** listener.c -- a datagram sockets "server" demo
 */

#include "server.h"
#define PACKETSIZE 513
#define WINDOWSIZE 16
#define MODULUS 32

char currPacket[PACKETSIZE];
char packetBuffer[1700][PACKETSIZE];
char *pt;

int packetsSent = -1;
int expectedSeqNum = 1;
int viewFirstTen = 0;
int maxPacket = 0;
int inTransit;
int numOfResets;
int arrayIndex;
int requestNumber;
int seqNum;
int base;
int sequenceMax;

void buildPacket(char *seqNum, char *checkSum, char *acknak, char *message);
char initialSend(char message[], char getChar, FILE *fr, char seqNumOut[],
		char checkSum[], char *acknak, int numBytes, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len);
//void reSend(int numBytes, int sockfd);

int main(void) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	int sequenceMax;

	srand(time(NULL));
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
		maxPacket = 0;
		requestNumber = 0;
		seqNum = 0;
		base = 0;
		sequenceMax = WINDOWSIZE - 1;
		numOfResets = 0;
		arrayIndex = 0;
		inTransit = 0;

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
	int packetsSent = 0;
	int packetsReceived = 0;
	FILE *fr;
	fr = fopen(fileName, "r");
	char message[MESSAGESIZE + 1];
	char getChar;
	int numBytes;
	char *acknak, *checkSum;

	acknak = "1";


	char seqNumOut[2];
	//acknak = getacknak();
	//checkSum = getCheckSum();
	//seqNum = getSeqNum();

	while (getChar != EOF) {

		puts("Beginning GBN");

		getChar = initialSend(message, getChar, fr, seqNumOut, checkSum, acknak,
				numBytes, sockfd, their_addr, addr_len);

		if (WINDOWSIZE - 1 > maxPacket) {
			sequenceMax = maxPacket;
		} else {
			sequenceMax = WINDOWSIZE - 1;
		}

	}

	for (;;) {
		int i = 0;
		printf(
				"ArrayIndex is currently %d, and SequenceMax is currently %d, and MaxPacket is %d\n",
				arrayIndex, sequenceMax, maxPacket);
		if (inTransit == 0) {
			inTransit = 1;
			if (sequenceMax != maxPacket && (sequenceMax - WINDOWSIZE) != arrayIndex)
			{
				sequenceMax = (arrayIndex + WINDOWSIZE - 1);
				arrayIndex = (sequenceMax - WINDOWSIZE + 1);
			}
			for (i = arrayIndex; i <= sequenceMax; i++) {
				if (strlen(packetBuffer[i]) > 0) {


					char message[MESSAGESIZE + 1], myChecksum[3], tempAcknack[2];
					int skip = 5;

					memset(message, 0, MESSAGESIZE + 1);
					memset(myChecksum, 0, 3);
					memset(tempAcknack, 0, 2);
					memset(seqNumOut, 0, 3);



					strncpy(message, packetBuffer[i] + HEADERSIZE, strlen(packetBuffer[i]) - HEADERSIZE);


					pt = message;

					myChecksum[0] = packetBuffer[i][2];
					myChecksum[1] = packetBuffer[i][3];
					tempAcknack[0] = packetBuffer[i][4];
					seqNumOut[0] = packetBuffer[i][0];
					seqNumOut[1] = packetBuffer[i][1];


					skip = gremlin(&pt, 0.3, 0.2, 0.2, 0, rand() % 100);
					buildPacket(seqNumOut, myChecksum, tempAcknack, message);

					if(skip == PACKET_PASSED){
						if ((numBytes = sendto(sockfd, currPacket,
								strlen(currPacket), 0,
								(struct sockaddr *) &their_addr, addr_len)) == -1) {
							perror("talker: sendto");
						}
					}
					else{
						//puts("Packet Dropped");
					}

				}
			}
		}

		//===================================================================
		char incSeqNum[2];
		char incAckNak;
		char incmessage[6];
		memset(incmessage, 0, sizeof(incmessage));

		if ((numBytes = recvfrom(sockfd, incmessage, 5, 0,
				(struct sockaddr *) &their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			puts("The packet appears to have been lost.");
		}
		//printf("Incmessage = %s\n", incmessage);

		incAckNak = incmessage[4];
		incSeqNum[0] = incmessage[0];
		incSeqNum[1] = incmessage[1];
		//printf("IncSeqNum = %s\n", incSeqNum);
		int incSeqNumInt;
		//base = 0;
		requestNumber = atoi(incSeqNum);
		//printf("IncAckNak = %s\n ExpectedSeqNum = %d\n IncSeqNumInt = %d\n", incmessage, expectedSeqNum, incSeqNumInt);

		if (incAckNak == ACK
						&& (requestNumber > base
								|| ((base - WINDOWSIZE) > requestNumber))) //Received Expected ACK
						//|| (expectedSeqNum < incSeqNumInt) // Received Cumulative ACK


						//) //Received Overlapped Cumulative ACK
						{
					//	puts("Case 1 = Everything is fine, nothing is wrong");
					sequenceMax++;
					//= sequenceMax + (requestNumber - base);
					if (sequenceMax > maxPacket) {
						sequenceMax = maxPacket;
					}
					//printf("Base is currently %d, and requestNumber is %d\n", base, requestNumber);
					if ((base - WINDOWSIZE) > requestNumber) {
						puts("Incrementing reset counter");
						numOfResets++;
					}


					base = requestNumber;
					arrayIndex = base + (numOfResets * MODULUS);
					if (sequenceMax != maxPacket && (sequenceMax - WINDOWSIZE) != arrayIndex)
					{
						sequenceMax = (arrayIndex + WINDOWSIZE - 1);
						arrayIndex = (sequenceMax - WINDOWSIZE + 1);
					}
					inTransit = 1;

					if (strlen(packetBuffer[sequenceMax]) > 0) {

						char message[MESSAGESIZE + 1], myChecksum[3], tempAcknack[2];
						int skip = 0;

						memset(message, 0, MESSAGESIZE + 1);
						memset(myChecksum, 0, 3);
						memset(tempAcknack, 0, 2);
						memset(seqNumOut, 0, 3);


						strncpy(message, packetBuffer[sequenceMax] + HEADERSIZE, strlen(packetBuffer[sequenceMax]) - HEADERSIZE);

						pt = message;

						myChecksum[0] = packetBuffer[sequenceMax][2];
						myChecksum[1] = packetBuffer[sequenceMax][3];
						tempAcknack[0] = packetBuffer[sequenceMax][4];
						seqNumOut[0] = packetBuffer[sequenceMax][0];
						seqNumOut[1] = packetBuffer[sequenceMax][1];

						skip = gremlin(&pt, 0.3, 0.2, 0.2, 0, rand() % 100);
						buildPacket(seqNumOut, myChecksum, tempAcknack, message);

						if(skip == PACKET_PASSED){
							if ((numBytes = sendto(sockfd, currPacket,
									strlen(currPacket), 0,
									(struct sockaddr *) &their_addr, addr_len)) == -1) {
								perror("talker: sendto");
							}
						}
						else{
							//puts("Packet Dropped");
						}

					}
				}


				else {
					inTransit = 0;
				}


	}
	//================================================================================================

	puts("Response received...");

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
char initialSend(char message[], char getChar, FILE *fr, char seqNumOut[],
		char checkSum[], char *acknak, int numBytes, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len) {
	memset(message, 0, MESSAGESIZE + 1);
	memset(seqNumOut, 0, sizeof(seqNumOut));

	int messageLength = 0;
	while (messageLength < MESSAGESIZE && (getChar = fgetc(fr)) != EOF) {
		message[messageLength] = getChar;
		messageLength++;
	}


	int seqNumMod = seqNum % MODULUS;
	sprintf(seqNumOut, "%ld", seqNumMod);


	if (seqNumMod < 10) {
		seqNumOut[0] = '0';
		seqNumOut[1] = (char) (((int) '0') + seqNumMod);
	}

	seqNum++;
	printf("Writing sequence number %s into packet.\n", seqNumOut);

	checkSum = checksum(message, checkSum);

	buildPacket(seqNumOut, checkSum, acknak, message);
	memset(packetBuffer[maxPacket], 0, sizeof(packetBuffer[maxPacket]));
	strcpy(packetBuffer[maxPacket], currPacket);


	if (strlen(currPacket) < 512) {
		packetBuffer[maxPacket][4] = '2';
	}
	maxPacket++;
	return getChar;
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
