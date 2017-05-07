#ifndef SENDLLDPRAWSOCK_H
#define SENDLLDPRAWSOCK_H

#include <sys/ioctl.h>
#include <net/if.h>
#include "openssl_sign.h"

int sendLLDPrawSock (int LLDPDU_len, char *lanbeaconCustomTLVs,
					struct open_ssl_keys *lanbeacon_keys);
struct received_lldp_packet *recLLDPrawSock(struct open_ssl_keys *lanbeacon_keys);
unsigned long receiveChallenge();
void getInterfaces (int *sockfd, int *numInterfaces, unsigned short etherType, 
					unsigned short sendOrReceive, struct ifreq *if_idx, 
					struct ifreq *if_mac, int *sockopt, int *maxSockFd);
void sendRawSocket (unsigned char *destination_mac, void *payload, int payloadLen, 
					unsigned short etherType, struct open_ssl_keys *lanbeacon_keys);

#endif
