#ifndef EVALUATELANBEACON_H
#define EVALUATELANBEACON_H
#include "define.h"
#include "openssl_sign.h"


struct received_lldp_packet {
	unsigned char lldpReceivedPayload[LLDP_BUF_SIZ];
	ssize_t payloadSize;
	unsigned long challenge;
	unsigned char current_destination_mac[6];
};

char ** evaluatelanbeacon (struct received_lldp_packet *my_received_lldp_packet);
void bananaPIprint (char **parsedBeaconContents, struct open_ssl_keys *lanbeacon_keys);

#endif
