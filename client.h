#include "packet.h"
#include "gremlin.h"
#include "checksum.h"
#include "debug.h"


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

#define SERVERPORT "10030"
#define MAXBUFLEN 513
#define MESSAGELENGTH 507
#define HEADERSIZE 5

int main(int argc, char *argv[]);
int transfer(char packet[513], struct addrinfo *p, int sockfd, char *hostname);
int requestFile(char *fileName, struct addrinfo *p, int sockfd, char *hostname);
char * getCommand();
void receiveFile(struct addrinfo *p, int sockfd, char *fileName);
char *getMessage(char incMessage[513]);
