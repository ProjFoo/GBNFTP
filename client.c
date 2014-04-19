#include "client.h"


int main(int argc, char *argv[]){
	int sockfd;
	    struct addrinfo hints, *servinfo, *p;
	    int rv;
	    int numbytes;
	    char *fileName, *hostName;
	    char packet[MAXBUFLEN];
	    FILE *fr;

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

	int numBytes, MESSAGELENGTH;
	MESSAGELENGTH = 494;
	char incMessage[513];
	char message [MESSAGELENGTH];
	size_t len;

	strsep(&fileName, ",");

	FILE *fr = fopen(&fileName[0], "w");

	struct sockaddr_storage their_addr;
				socklen_t addr_len;

	addr_len = sizeof their_addr;

	do{

		memset(incMessage, 0, MAXBUFLEN);

		memset(message, 0, MESSAGELENGTH);


		puts("Waiting for file data...");

		if ((numBytes = recvfrom(sockfd, incMessage, MAXBUFLEN , 0,
					(struct sockaddr *)&their_addr, &addr_len)) == -1) {
					perror("recvfrom");
					puts("The packet appears to have been lost.");
				}

		strcpy(message, incMessage);
		fprintf(fr, "%s", message);

		len = strlen(incMessage);
		}while(len > 18);

	fclose(fr);

}

char *getMessage(char incMessage[513]){
	char *message;
	int HEADER = 18;
	int i = 0;
	for(i = 18; i < 512;i++){
		message[i - HEADER] = incMessage[i];
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



