#ifndef SENDLLDPRAWSOCK_H
#define SENDLLDPRAWSOCK_H

#include <sys/ioctl.h>
#include <net/if.h>
#include "openssl_sign.h"
#include "receiver.h"
#include "sender.h"

#define SEND_SOCKET 0
#define REC_SOCKET 1

void new_lldp_receiver (struct receiver_information *my_receiver_information);

int sendLLDPrawSock (struct sender_information *my_sender_information);

struct received_lldp_packet *recLLDPrawSock(struct receiver_information *my_receiver_information);

unsigned long receiveChallenge(int *sockfd, int numInterfaces, int maxSockFd, char *challenge_dest_mac);

void getInterfaces (int *sockfd, int *numInterfaces, unsigned short etherType, 
					unsigned short sendOrReceive, struct ifreq *if_idx, 
					struct ifreq *if_mac, int *sockopt, int *maxSockFd, char *interface_to_send_on);

void sendRawSocket (unsigned char *destination_mac, void *payload, int payloadLen, 
					unsigned short etherType, struct open_ssl_keys *lanbeacon_keys, char *interface_to_send_on);

void flush_all_interfaces (int *sockfd, int maxSockFd, int numInterfaces);

#endif
