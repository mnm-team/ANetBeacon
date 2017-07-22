#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H
#include "define.h"
#include "openssl_sign.h"

struct sender_information {
	
	char *lanBeacon_PDU;
	int lan_beacon_pdu_len;
//	int generate_keys;
	int send_frequency;
	char *interface_to_send_on;
	struct open_ssl_keys lanbeacon_keys;
};

char *mergedlanbeaconCreator (int *argc, char **argv, struct sender_information *my_sender_information);
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, 
										char **combinedString, char *source, 
										char *combinedBeacon, int *currentByte);
void transferToCombinedBeacon (unsigned char subtype, void *source, char *combinedBeacon, 
								int *currentByte, unsigned short int currentTLVlength);
void transferToCombinedString (char *TLVdescription, char **combinedString, char *source);
void ipParser (int ip_V4or6, char *optarg, char **combinedString, 
				char *mylanbeacon, int *currentByte);
void printHelp();

#endif
