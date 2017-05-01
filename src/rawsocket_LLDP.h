#ifndef SENDLLDPRAWSOCK_H
#define SENDLLDPRAWSOCK_H

#include "openssl_sign.h"

int sendLLDPrawSock (int LLDPDU_len, char *LANbeaconCustomTLVs, struct open_ssl_keys *lanbeacon_keys);
struct received_lldp_packet *recLLDPrawSock(struct open_ssl_keys *lanbeacon_keys);
void sendChallenge (unsigned char *destination_mac, unsigned long challenge);
unsigned long receiveChallenge();

#endif
