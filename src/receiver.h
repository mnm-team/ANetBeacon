#ifndef EVALUATELANBEACON_H
#define EVALUATELANBEACON_H
#include "define.h"
#include "openssl_sign.h"


struct receiver_information {
	
	int authenticated;
	int scroll_speed;
	int currentLLDPDU_for_printing;
	
	int number_of_currently_received_packets;
	struct received_lldp_packet *pointers_to_received_packets[20];
	
	struct open_ssl_keys lanbeacon_keys;
};

struct received_lldp_packet {
	unsigned char lldpReceivedPayload[LLDP_BUF_SIZ];
	ssize_t payloadSize;
	unsigned long challenge;
	unsigned char current_destination_mac[6];
	
	char ** parsedBeaconContents;
};

char ** evaluatelanbeacon (struct received_lldp_packet *my_received_lldp_packet);
void bananaPIprint (struct receiver_information *my_receiver_information);

#endif
