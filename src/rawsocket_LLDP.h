#ifndef SENDLLDPRAWSOCK_H
#define SENDLLDPRAWSOCK_H

int sendLLDPrawSock (int LLDPDU_len, char *LANbeaconCustomTLVs);
void recLLDPrawSock(unsigned char *LLDPreceivedPayload, ssize_t *payloadSize);

#endif