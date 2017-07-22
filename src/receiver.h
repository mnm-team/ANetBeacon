#ifndef EVALUATELANBEACON_H
#define EVALUATELANBEACON_H
#include "define.h"
#include "openssl_sign.h"
#include "rawsocket_LAN_Beacon.h"



struct received_lan_beacon_packet {
	unsigned char lan_beacon_ReceivedPayload[LAN_BEACON_BUF_SIZ];
	ssize_t payloadSize;
	unsigned long challenge;
	unsigned char current_destination_mac[6];
	
	int successfullyAuthenticated;
	
	int times_left_to_display;
	
	char ** parsedBeaconContents;
};

struct receiver_interfaces {
	int sockfd[20];
	int sockopt[20];
	int maxSockFd;
	int numInterfaces;
};

struct receiver_information {
	
	int authenticated;
	int scroll_speed;
	int current_lan_beacon_pdu_for_printing;
	
	int number_of_currently_received_packets;
	struct received_lan_beacon_packet *pointers_to_received_packets[20];
	
	struct open_ssl_keys lanbeacon_keys;
	struct receiver_interfaces my_receiver_interfaces;
};

char ** evaluatelanbeacon (struct received_lan_beacon_packet *my_received_lan_beacon_packet, struct open_ssl_keys *lanbeacon_keys);
void bananaPIprint (struct receiver_information *my_receiver_information);

#endif
