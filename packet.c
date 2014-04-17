struct packet {
    char seqNum[2];
    char checksum[16];
    char message[494];
}
