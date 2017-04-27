#ifndef EVALUATELANBEACON_H
#define EVALUATELANBEACON_H

struct received_lldp_packet {
	unsigned char lldpReceivedPayload[LLDP_BUF_SIZ];
	ssize_t payloadSize;
};

char ** evaluateLANbeacon (unsigned char *LLDPreceivedPayload, ssize_t payloadSize);
void bananaPIprint (char **parsedBeaconContents, struct open_ssl_keys *lanbeacon_keys);

#endif
