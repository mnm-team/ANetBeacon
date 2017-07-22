#ifndef RAWSOCKET_LAN_BEACON_H
#define RAWSOCKET_LAN_BEACON_H

#include <sys/ioctl.h>
#include <net/if.h>
#include "openssl_sign.h"
#include "receiver.h"
#include "sender.h"

#define SEND_SOCKET 0
#define REC_SOCKET 1

void new_lan_beacon_receiver (struct receiver_information *my_receiver_information);

int send_lan_beacon_rawSock (struct sender_information *my_sender_information);

unsigned long receiveChallenge(int *sockfd, int numInterfaces, int maxSockFd, 
				char *challenge_dest_mac, struct sender_information *my_sender_information);

void getInterfaces (int *sockfd, int *numInterfaces, unsigned short etherType, 
					unsigned short sendOrReceive, struct ifreq *if_idx, 
					struct ifreq *if_mac, int *sockopt, int *maxSockFd, char *interface_to_send_on);

void sendRawSocket (unsigned char *destination_mac, void *payload, int payloadLen, 
					unsigned short etherType, struct open_ssl_keys *lanbeacon_keys, 
					char *interface_to_send_on, struct sender_information *my_sender_information);

void flush_all_interfaces (int *sockfd, int maxSockFd, int numInterfaces);

#endif
