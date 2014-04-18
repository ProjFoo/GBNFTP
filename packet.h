#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED
#define PACKETSIZE 513
#define SEQNUMSIZE 2
#define CHECKSUMSIZE 16
#define MESSAGESIZE 494

struct packet {
    char seqNum[2];
    char checksum[16];
    char message[494];
};

#endif
