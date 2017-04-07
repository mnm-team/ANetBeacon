#ifndef SENDLLDPRAWSOCK_H
#define SENDLLDPRAWSOCK_H

int sendLLDPrawSock (int LLDPDU_len, char *LANbeaconCustomTLVs);
void recLLDPrawSock(int argc, char *argv[], unsigned char *LLDPreceivedPayload, ssize_t *payloadSize);

#endif
