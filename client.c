#include "client.h"
int viewFirstTen; //DELETE ME


int main(int argc, char *argv[]){
	int sockfd;
	    struct addrinfo hints, *servinfo, *p;
	    int rv;
	    int numbytes;
	    char *fileName, *hostName;
	    char packet[MAXBUFLEN];
	    FILE *fr;
	    
	    srand(time(NULL));
	    hostName = malloc(strlen(argv[1]));

	    strcpy(hostName, argv[1]);
	    fileName = malloc(100);
	    strcpy(fileName, getCommand());

	    if (argc != 2) {
	        fprintf(stderr,"usage: talker hostname message\n");
	        exit(1);
	    }

	    memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_DGRAM;

	    if ((rv = getaddrinfo(hostName, SERVERPORT, &hints, &servinfo)) != 0) {
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


	    if(requestFile(fileName, p, sockfd, hostName) == 1){
	    	puts("File request successful! \nBegin receiving data.");
	    	receiveFile(p, sockfd, fileName);
	    }
	    else{
	    	puts("File does not exist.");
	    }


	    freeaddrinfo(servinfo);

	    close(sockfd);
	    free(fileName);

	    return 0;
	}

void receiveFile(struct addrinfo *p, int sockfd, char *fileName){

	int numBytes;

	char incMessage[MAXBUFLEN];
	char message [MESSAGELENGTH];
	size_t len;

	strsep(&fileName, ",");

	//FILE *fr = fopen(&fileName[0], "w");
	FILE *fr = fopen("test.txt", "w");

	struct sockaddr_storage their_addr;
				socklen_t addr_len;

	addr_len = sizeof their_addr;

	char incacknak = FIN;
	do{

		memset(incMessage, 0, MAXBUFLEN);
		memset(message, 0, MESSAGELENGTH);


		puts("Waiting for file data...");

		if ((numBytes = recvfrom(sockfd, incMessage, MAXBUFLEN , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			puts("The packet appears to have been lost.");
		}
		incacknak = incMessage[4];
		printf("IncAckNak = %c\n", incacknak);
		len = strlen(incMessage);
		puts("Data received! Saving to file...");
		printf("LEN = %d\n", len);
		//if(len > HEADERSIZE){

			///puts("about to strcopy");
			//printf("IncMessage = %s\n", incMessage);
			strncpy(message, incMessage + HEADERSIZE, strlen(incMessage) - HEADERSIZE);

			///puts("finished strcopy");
			if (viewFirstTen < 11)
			{
			//printf("Message = %s\n", message);
			viewFirstTen++;
			}
			//Do checksum stuff
			//Do GBN stuff
			//Do SeqNum stuff
			

			char tempSeqNum[2];
			tempSeqNum[0] = incMessage[0];
			tempSeqNum[1] = incMessage[1];			
			int incSeqNum = atoi(tempSeqNum);
			int validChecksum = 0;
			//printf("IncSeqNum = %d\n", incSeqNum);
			int expectedSeqNum = seqNum;
			printf("\nSeqNum: %c%c\n IncSeqNum = %d\n Expected SeqNum = %d\n", incMessage[0], incMessage[1], incSeqNum, expectedSeqNum);

			char acknak[1];
			char seqNumOut[2];			
			char myChecksum[3];
			char *checkChecksum;

			checkChecksum = malloc(3);
			memset(checkChecksum, 0, 3);
			memset(myChecksum, 0, 3);

			checkChecksum = checksum(message, checkChecksum);
			myChecksum[0] = incMessage[2];
			myChecksum[1] = incMessage[3];

			if(strcmp(checkChecksum, myChecksum) == 0){
				puts("Successful Checksum");
				validChecksum = 1;
			}
			else{
				puts("Corrupt Checksum");
				validChecksum = 0;
			}

			
			int v1 = rand() % 100;
			//int v1 = 100;
			if (v1 > 70 && expectedSeqNum == incSeqNum && incacknak != FIN)
			{
				puts("Case 1. Everything is fine, nothing is wrong");
				acknak[0] = ACK;				
				sprintf(seqNumOut, "%ld", (expectedSeqNum+1));
				if (expectedSeqNum < 9)
				{
					seqNumOut[1] = seqNumOut[0];
					seqNumOut[0] = '0';
				}
				//printf("SeqNumOut = %s\n", seqNumOut);
				seqNum++;
				fprintf(fr, "%s", message);
			}
			
			else if (expectedSeqNum != incSeqNum)
			{
				puts("Case 3. OOO packet");
				acknak[0] = ACK;
				sprintf(seqNumOut, "%ld", expectedSeqNum);
				if (expectedSeqNum < 10)
				{
					seqNumOut[1] = seqNumOut[0];
					seqNumOut[0] = '0';
				}
				//printf("SeqNumOutOops = %s\n", seqNumOut);
			}
			
			else if (v1 <= 70) // If checksum fails
			{
				puts("Case 2. Corrupt packet");

				acknak[0] = NAK;
				sprintf(seqNumOut, "%ld", expectedSeqNum);
				if (expectedSeqNum < 10)
				{
					seqNumOut[1] = seqNumOut[0];
					seqNumOut[0] = '0';
				}
			}



			else if (expectedSeqNum == incSeqNum && incacknak == FIN)
			{
				puts("finishing up");
				fprintf(fr, "%s", message);
				break;
			}
			/*
			int v1 = rand() % 100;
			if (v1 < 50)
			{
				puts("Boom goes the dynamite");
				acknak[0] = NAK;
			}
			*/
			buildPacket(seqNumOut, myChecksum, acknak);
			
			if ((numBytes = sendto(sockfd, currPacket, strlen(currPacket), 0,
										(struct sockaddr *)&their_addr, addr_len)) == -1) {
											perror("talker: sendto");
								}
			
			//if all success, print to file
			//fprintf(fr, "%s", message);
		//}





		}while(1);
/*
	char finalSeqNum[2] = "00";
	char finalCheckSum[2] = "11";
	char finalAckNak[1];
	finalAckNak[0] = FIN;
	buildPacket(finalSeqNum, finalCheckSum, finalAckNak);
	if ((numBytes = sendto(sockfd, currPacket, strlen(currPacket), 0,
								(struct sockaddr *)&their_addr, addr_len)) == -1) {
									perror("talker: sendto");
	}
	*/
	puts("Finished receiving file. Saving and closing.");

	fclose(fr);

}

char *getMessage(char incMessage[MAXBUFLEN]){
	char *message;

	int i;
	for(i = HEADERSIZE; i < MAXBUFLEN - 1;i++){
		message[i - HEADERSIZE] = incMessage[i];
	}



	return message;
}

char * getCommand(){
	char *fileName, *command, *returnMes;

	puts("What do you want to do?");
	fileName = malloc(50);
	command = malloc(50);
	scanf("%s", command);

	while(strcmp(command, "GET") != 0){
		puts("Command not support. ctrc-c to end or enter another command");
		scanf("%s", command);
	}

	puts("What file would you like to get?");
	scanf("%s", fileName);
	puts("");

	strcpy(returnMes, "GET,");
	strcat(returnMes, fileName);

	return returnMes;

}

int transfer(char packet[513], struct addrinfo *p, int sockfd, char *hostname){

	return 0;

}

int requestFile(char *fileName, struct addrinfo *p, int sockfd, char *hostname){

	char incMessage[MAXBUFLEN];
	int numBytes, valid;
	char packet[strlen(fileName) + 4];

	valid = 0;

	struct sockaddr_storage their_addr;
			socklen_t addr_len;

	//strcpy(packet, "GET,");
	//strcat(packet, fileName);
	strcpy(packet, fileName);

	printf("File request sent...");

	if ((numBytes = sendto(sockfd, packet, strlen(packet), 0,
		             p->ai_addr, p->ai_addrlen)) == -1) {
		        perror("talker: sendto");
		        exit(1);
		    }

	puts("Waiting for confirmation...");
	addr_len = sizeof their_addr;

	if ((numBytes = recvfrom(sockfd, incMessage, MAXBUFLEN , 0,
	    		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
	    		perror("recvfrom");
	    		puts("The packet appears to have been lost.");
	    	}


	if(strcmp(incMessage, "invalid") == 0){

	}
	else{

		valid = 1;
	}

	return valid;
}


void buildPacket(char *seqNum, char *checkSum, char *acknak)
{
	//puts("\nZeroing out current packet before building.");
	memset(currPacket, 0, sizeof(currPacket));
	//puts("Copying sequence number into packet");
	//printf("SeqNumOutBuild = %s\n", seqNum);
	strcpy(currPacket, seqNum);
	//puts("Concatenating Checksum into packet");
	strcat(currPacket, checkSum);
	//puts("Concatenating acknak into packet");
	strcat(currPacket, acknak);
	//printf("Message = %s\n", message);
	//puts("Packet built and ready for transfer!\n");
}
