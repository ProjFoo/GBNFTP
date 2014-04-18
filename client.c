#include "client.h"
#include "GBNLibrary.c"

int main(int argc, char *argv[]){
	int sockfd;
	    struct addrinfo hints, *servinfo, *p;
	    int rv;
	    int numbytes;
	    char *fileName, *command, *hostName;
	    FILE *fr;

	    hostName = malloc(strlen(argv[1]));

	    strcpy(hostName, argv[1]);

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


	    if (argc != 3) {
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
	    	//receiveFile()
	    }






	    freeaddrinfo(servinfo);

	    close(sockfd);

	    return 0;
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

	strcpy(packet, "GET,");
	strcat(packet, fileName);

	printf("Message sent: %s\n", packet);

	if ((numBytes = sendto(sockfd, packet, strlen(packet), 0,
		             p->ai_addr, p->ai_addrlen)) == -1) {
		        perror("talker: sendto");
		        exit(1);
		    }

	puts("Waiting to Receive...");
	addr_len = sizeof their_addr;

	if ((numBytes = recvfrom(sockfd, incMessage, MAXBUFLEN , 0,
	    		(struct sockaddr *)&their_addr, &addr_len)) == -1) {
	    		perror("recvfrom");
	    		puts("The packet appears to have been lost.");
	    	}


	if(strcmp(incMessage, "invalid") == 0){
		puts("File does not exist.");
	}
	else{
		valid = 1;
	}

	return valid;
}



