#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED

struct packet {
    char seqNum[2];
    char checksum[16];
    char message[494];
};

#endif
