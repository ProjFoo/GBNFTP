/*
** listener.c -- a datagram sockets "server" demo
*/

#include "server.h"


int main(void)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char *buf;
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];
    FILE *fr;

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

    printf("listener: waiting to recvfrom...\n");

    addr_len = sizeof their_addr;

    while(1){
    	char *fileName;
		if ((numbytes = recvfrom(sockfd, buf, strlen(buf) , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		//fileName = checkForGet(buf);
		strcpy(fileName, checkForGet(buf));

		fr = fopen(fileName, "r");
		if (fr == NULL){
			replyWithInvalidFile(p, sockfd);
		}

    }

    close(sockfd);

    return 0;
}

void replyWithInvalidFile(struct addrinfo *p, int sockfd){

	int numBytes;
	char message[8] = "invalid";
	struct sockaddr_storage their_addr;
				socklen_t addr_len;

	if ((numBytes = sendto(sockfd, message, strlen(message), 0,
									(struct sockaddr *)&their_addr, addr_len)) == -1) {
										perror("talker: sendto");
							}
}

char* checkForGet(char *packetIn){
	char *splitString;
	char *cmp;
	int valid = 0;
	char COMMAND[3] = "GET";

	splitString = strsep(&packetIn, ",");

	sscanf(cmp, "%s", &packetIn[0]);

	if(strcmp(packetIn,COMMAND)){
		valid = 1;
	}

	return cmp;
}
