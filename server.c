/*
 ** listener.c -- a datagram sockets "server" demo
 */

#include "server.h"
#define PACKETSIZE 513
#define WINDOWSIZE 16
#define MODULUS 32

char currPacket[PACKETSIZE];
char packetBuffer[MODULUS][PACKETSIZE];


int isSent = 1;
int seqNum = MODULUS;
int packetsSent = -1;
int base = 0;
int expectedSeqNum = 1;
int viewFirstTen = 0;

void buildPacket(char *seqNum, char *checkSum, char *acknak, char *message);
char initialSend(char message[], char getChar, FILE *fr, char seqNumOut[],
		char checkSum[], char *acknak, int numBytes, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len);
void reSend(int numBytes, int sockfd);

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
	//acknak = getacknak();
	//checkSum = getCheckSum();
	//seqNum = getSeqNum();

	while (getChar != EOF) {  //while there is file to be sent

		while(/*there is stuff in the recv socket */ 0) {
			/*recv that stuff from the recv socket */
			if( requestNumber > sequenceBase ){
				sequenceMax = sequenceMax + (requestNumber - sequenceBase);
				sequenceBase = requestNumber;
			}
		}

		for(sequenceNumber = sequenceBase; sequenceNumber < sequenceMax; sequenceNumber++) {
			if(/*packetList[sequenceNumber] does not exist*/ 0) {
				//build packetList[sequenceNumber]
			}
			//send packetList[sequenceNumber]
			if(/*there is stuff in the recv socket*/ 0){
				//get stuff from recv socket
				//parse stuff from recv socket
			}
		}

		puts("Beginning GBN");

		int i = 0;
		if (isSent > 0) {
			isSent = 0;
			for (i = 0; i < WINDOWSIZE && getChar != EOF; i++) {
				getChar = initialSend(message, getChar, fr, seqNumOut, checkSum,
						acknak, numBytes, sockfd, their_addr, addr_len);
				packetsSent++;
			}
			puts("Packet sent...Waiting for response...");

		}
		//reSend(numBytes, sockfd);

		//put in receive here

		while (isSent != 1) {
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
			int base = 0;
			incSeqNumInt = atoi(incSeqNum);
			//printf("IncAckNak = %s\n ExpectedSeqNum = %d\n IncSeqNumInt = %d\n", incmessage, expectedSeqNum, incSeqNumInt);

			if (incAckNak == ACK && ((expectedSeqNum == incSeqNumInt))) //Received Expected ACK
			//|| (expectedSeqNum < incSeqNumInt) // Received Cumulative ACK
					//|| ((expectedSeqNum - WINDOWSIZE) > incSeqNumInt))
					//) //Received Overlapped Cumulative ACK
					{
				puts("Case 1 = Everything is fine, nothing is wrong");
				expectedSeqNum = incSeqNumInt + 1;
				base++;
				if (expectedSeqNum >= 32) {
					expectedSeqNum = 0;
				}
				if (base >= WINDOWSIZE) {
					base = 0;
				}
				packetsReceived++;
				if (packetsSent == packetsReceived) {
					isSent = 1;
				}
				printf("\n\n\n\n\n\n\n\n\nPacketsReceived = %d\n\n\n\n\n\n\n\n\n\n\n", packetsReceived);
			}

			else //if ((incAckNak == ACK && expectedSeqNum != incSeqNumInt) || incAckNak == NAK)
			{
				//puts("Case 2 = Received Out Of Order ACK, or received NAK");
				int i;
				int j = incSeqNumInt;
				/*printf(
				 "IncAckNak = %c\n ExpectedSeqNum = %d\n IncSeqNumInt = %d\n IsSent = %d\n",

				 incAckNak, expectedSeqNum, incSeqNumInt, isSent);
				 */
				for (i = base; i < WINDOWSIZE && strlen(packetBuffer[j]) != 0;
						i++) {
					if ((numBytes = sendto(sockfd, packetBuffer[j],
							strlen(packetBuffer[j]), 0,
							(struct sockaddr *) &their_addr, addr_len)) == -1) {
						perror("talker: sendto");
					}
					//printf("\n\nPacketBuffer = \n%s\n\n", packetBuffer[j]);
					j++;
					j = j % MODULUS;
					isSent = 0;
				}
			}
		}
		//================================================================================================

		puts("Response received...");
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

/*
 void reSend(int numBytes, int sockfd)
 {
 //===================================================================
 char incSeqNum[2];
 char incAckNak;
 char incmessage[6];
 memset(incmessage, 0, sizeof(incmessage));

 struct sockaddr_storage their_addr;
 socklen_t addr_len;

 if ((numBytes = recvfrom(sockfd, incmessage, 5, 0,
 (struct sockaddr *)&their_addr, &addr_len)) == -1)
 {
 perror("recvfrom");
 puts("The packet appears to have been lost.");
 }
 //printf("Incmessage = %s\n", incmessage);

 incAckNak = incmessage[4];
 incSeqNum[0] = incmessage[0];
 incSeqNum[1] = incmessage[1];
 //printf("IncSeqNum = %s\n", incSeqNum);
 int incSeqNumInt;
 int base = 0;
 incSeqNumInt = atoi(incSeqNum);
 //printf("IncAckNak = %s\n ExpectedSeqNum = %d\n IncSeqNumInt = %d\n", incmessage, expectedSeqNum, incSeqNumInt);

 if (incAckNak == ACK &&
 ((expectedSeqNum == incSeqNumInt) //Received Expected ACK
 || (expectedSeqNum < incSeqNumInt) // Received Cumulative ACK
 || ((expectedSeqNum - WINDOWSIZE) > incSeqNumInt))) //Received Overlapped Cumulative ACK
 {
 //puts("Case 1 = Everything is fine, nothing is wrong");
 expectedSeqNum = incSeqNumInt+1;
 base++;
 if (expectedSeqNum >= 32){
 expectedSeqNum = 0;
 }
 if (base >= WINDOWSIZE)
 {
 base = 0;
 }
 }
 else //if ((incAckNak == ACK && expectedSeqNum != incSeqNumInt) || incAckNak == NAK)
 {
 //puts("Case 2 = Received Out Of Order ACK, or received NAK");
 int i;
 int j = incSeqNumInt;
 for(i = base; i < WINDOWSIZE; i++)
 {
 if ((numBytes = sendto(sockfd, packetBuffer[j], strlen(packetBuffer[j]), 0,
 (struct sockaddr *)&their_addr, addr_len)) == -1)
 {
 perror("talker: sendto");
 }
 //printf("PacketBuffer = %s\n", packetBuffer[j]);
 j++;
 j = j % MODULUS;

 }
 }

 //================================================================================================

 }
 */
char initialSend(char message[], char getChar, FILE *fr, char seqNumOut[],
		char checkSum[], char *acknak, int numBytes, int sockfd,
		struct sockaddr_storage their_addr, socklen_t addr_len) {
	memset(message, 0, MESSAGESIZE + 1);

	int messageLength = 0;
	while (messageLength < MESSAGESIZE && (getChar = fgetc(fr)) != EOF) {
		message[messageLength] = getChar;
		messageLength++;
	}
	//puts("Incrementing sequence#");
	int seqNumMod = seqNum % MODULUS;
	//puts("Casting sequence# to char array");			
	sprintf(seqNumOut, "%ld", seqNumMod);
	if (seqNumMod < 10) {
		seqNumOut[0] = '0';
		seqNumOut[1] = (char) (((int) '0') + seqNumMod);
	}
	seqNum++;
	//puts("Building packet");
	buildPacket(seqNumOut, checkSum, acknak, message);
	memset(packetBuffer[seqNumMod], 0, sizeof(packetBuffer[seqNumMod]));
	strcpy(packetBuffer[seqNumMod], currPacket);
	//sleep(1);
	//printf("Packet Buffer = %s\n", packetBuffer[seqNumMod]);
	//packetBuffer[seqNumMod] = currPacket;
	/*
	 if (viewFirstTen < 11)
	 {
	 printf("PacketBuffer = %s\n", packetBuffer[seqNumMod]);
	 viewFirstTen++;
	 }
	 */
	if ((numBytes = sendto(sockfd, packetBuffer[seqNumMod],
			strlen(packetBuffer[seqNumMod]), 0, (struct sockaddr *) &their_addr,
			addr_len)) == -1) {
		perror("talker: sendto");
	}

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
