#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H

char *mergedlanbeaconCreator (int *argc, char **argv, int *lldpdu_len, 
								struct open_ssl_keys *lanbeacon_keys, char **interface_to_send_on) ;
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
