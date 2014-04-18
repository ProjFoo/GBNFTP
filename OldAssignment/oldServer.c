/*
** listener.c -- a datagram sockets "server" demo
*/

#include "server.h"


int main(int argc, char *argv[])
{
	if (argv[2] != NULL){
		damaged = atof(argv[2]);
	}
	
	if (argv[3] != NULL)
	{
		lost = atof(argv[3]);
	}

	if (damaged > 0 && lost > 0 && (damaged + lost > 1.0))
		{
		puts("Damaged + lost probability cannot be > 0\n");
		exit(1);
		}

    struct addrinfo hints, *servinfo, *p;
    int rv, numbytes, sockfd;    
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];


    //============SERVER SETUP==========================
    srand(time(NULL));
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
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
	int sizeOfArray = 0;

	//======================END Server Setup=======================


	while(1){
		char *buf;
		printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof their_addr;
		buf = malloc(MAXBUFLEN);

		//fill buf will nul terminating strings
		int i;
		for(i = 0; i < MAXBUFLEN; i++){
			buf[i] = '\0';
		}


		//===========Initial recieve of request==============
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0,
			(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}

		printf("listener: packet is %d bytes long\n", numbytes);
	
		printf("listener: got packet from %s\n",
			inet_ntop(their_addr.ss_family,
				get_in_addr((struct sockaddr *)&their_addr),
				s, sizeof s));

		//=====================================================

		//========Write to File==================
		receive(numbytes, buf, p, sockfd);
		//=========================================

	}


    close(sockfd);

    return 0;
}


int receive(int numbytes, char *buf, struct addrinfo *p, int sockfd)
{
	socklen_t addr_len;
	int sizeOfBuf = 0;
	char *splitString;
	char *checksum;
	struct sockaddr_storage their_addr;
	char *acknak = malloc(3);
	acknak[2] = '\0';
	char *abp = malloc(2);
	abp[0] = '0';
	abp[1] = '\0';
	char	ACK = '1',
			NAK = '0';

	//Split the message to see what the command is
	splitString = strsep(&buf, ",");


		if(strlen(buf) > 1 ){

			char *cmp;
			cmp = malloc(sizeof(buf[0]));

			//Pull out the command to compare against PUT
			sscanf(cmp, "%s", &buf[0]);

			if(strcmp(buf,"PUT")){
				sscanf(cmp, "%s", &buf[1]);
				FILE *fr = fopen("test.txt", "w");

				puts("=====PUT initiated...========");

				char *message;
				buf = malloc(MAXBUFLEN);
				message = malloc(sizeOfBuf - 4);

				do{

					// ==================Recieve data from file=========================
					//
					// Recieves in the order of:
					// acknack, seqNum, checksum, body

					addr_len = sizeof their_addr;

					//Acknak stuff===================
					puts("--------------------------------");
					puts("Waiting for next packet...");
					if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN , 0,
						(struct sockaddr *)&their_addr, &addr_len)) == -1) {

						acknak[0] = NAK;

					}
					else
					{
						acknak[0] = ACK;
					}

					sizeOfBuf = strlen(buf);
					message = realloc(message, sizeOfBuf - 3);

					if(sizeOfBuf > 4)
					{

						int i = 0;
						for(i = 0; i < sizeOfBuf - 3; i++){
							message[i] = '\0';
						}
						memcpy(message, buf + 4, sizeOfBuf - 4);
					}

					if(sizeOfBuf > 4 && gremlin(damaged, lost, &message)){

						puts("*** Packet Received! ***");
						printf("Size of packet: %d\n", numbytes);



						//======== CHECKSUM =====================

						if(sizeOfBuf > 4){

							char *checkChecksum;
							checkChecksum = malloc(3);
							memcpy(checkChecksum, buf + 2, 2);
							checkChecksum[2] = '\0';

							checksum = malloc(3);

							checksum = makeChecksum(message);
							checksum[2] = '\0';


							printf("Recieved Checksum: %s\n", checkChecksum);
							printf("Calculated Checksum: %s\n", checksum);
							printf("Current Sequence Number: %c\n\n", abp[0]);

							if(strcmp(checksum, checkChecksum) == 0){
								puts("****Checksum Successful!****");
								char *msgABP;

								memcpy(msgABP, buf + 1, 1);




								if( strcmp(abp, msgABP) == 0 ){
									puts("****ABP Sequence fail!****\n");
									abp[1] = '\0';
									acknak[0] = NAK;
									acknak[1] = abp[0];

								} else {
									puts("****ABP Sequence Success!***\n");

									abp[0] = msgABP[0];
									abp[1] = '\0';

									acknak = malloc(3);
									acknak[0] = ACK;
									acknak[1] = abp[0];



									//======Write to file====================
									writeToFile(fr, buf, numbytes, sizeOfBuf);
									//=======================================
								}
							}
							else{
								puts("****Checksum failed*****!");
								acknak[0] = NAK;
							}

						}

						//=====Send ACK/NAK	=====================


						if ((numbytes = sendto(sockfd, acknak, strlen(acknak), 0,
								(struct sockaddr *)&their_addr, addr_len)) == -1) {
									perror("talker: sendto");
						}
						else{
							if (acknak[0] == ACK){

							printf("Sent ack/nack: %s\n\n", acknak);
							}
						}
						//=======================================

					}
					else{

					}

					int i = 0;
					for(i= 0; i < sizeOfBuf - 4; i++){
						buf[i] = '\0';
					}


				}while(sizeOfBuf > 4);
				free(message);

				puts("\nPUT successfully completed!\n");

				fclose(fr);
			}
			else{
				puts("Command not recognized!");
			}

		}
	return 0;
}

int gremlin(float damaged, float lost, char ** packet)
{

	int returnNum;
	int sizeOfPacket = strlen(*packet);
	int r = rand() % 101;
	int p_dam = damaged * 100,
		p_lost = lost * 100;


	if (r <= p_dam)
	{

		damage(packet);

		returnNum = 1;
	}
	else if (r <= p_dam + p_lost)
	{

		returnNum = 0;

	}
	else {
		returnNum = 1;
	}

	return returnNum;
}

char * damage(char **p)
{

	const int ONE = 7,
			TWO = 2,
			THREE = 1;

	char *msg = (*p);
	char corruptor = '0';
	int r = rand() % 10;



	if (r < ONE )
	{

		int s = rand() % sizeof(*p);
		msg[s] = corruptor;

	}

	else if(r < (ONE + TWO) )
	{

		int s = rand() % 124;
		msg[s] = corruptor;
		s = rand() % 124;
		while (msg[s] == corruptor)
			{
			s = rand() % 124;
			msg[s] = corruptor;
			s = rand() % 124;
			}
	}
	else if (r < ONE + TWO + THREE)
	{

		int s = rand() % 124;
		msg[s] = corruptor;

		while (msg[s] == corruptor)
			{
				s = rand() % 124;
				msg[s] = corruptor;
				s = rand() % 124;
			}
		while (msg[s] == corruptor)
			{
				s = rand() % 124;
				msg[s] = corruptor;
				s = rand() % 124;
			}
	}

	else
	{

	}
	return *p;
}


void writeToFile(FILE *fr, char *buf, int numbytes, int sizeOfBuf){
	if(strlen(buf) > 4){

		char *message;
		message = malloc(sizeOfBuf - 4);
		memcpy(message, buf + 4, sizeOfBuf - 4);
		fprintf(fr, "%s", message);

		if(sizeOfBuf > 44){

			message = malloc(sizeOfBuf-3);
			memcpy(message, buf + 4, 40);
			message[sizeOfBuf-1] = '\0';


			puts("===Message====");
			printf("%s\n", message);
			puts("===End Message===\n");

		}
		else{

			message = malloc(sizeOfBuf-4);
			memcpy(message, buf + 4, sizeOfBuf-3);
			message[sizeOfBuf-1] = '\0';

			printf("Size of packet: %d\n", numbytes);
			puts("===Message====");
			printf("%s\n", message);
			puts("===End Message===");


		}
	}

}

char* makeChecksum(char* message)
{
    int sum = 0;
    char *mysum = malloc(20),
    	 *checksum = malloc(3);

    checksum[2] = '\0';
	int i = 0;
	for( i = 0; i <= strlen(message); i++)
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

