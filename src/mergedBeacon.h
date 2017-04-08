#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H

// Subtype numbers LANbeacon:
#define SUBTYPE_VLAN_ID 200
#define SUBTYPE_NAME 201
#define SUBTYPE_CUSTOM 202
#define SUBTYPE_IPV4 203
#define SUBTYPE_IPV6 204
#define SUBTYPE_EMAIL 205
#define SUBTYPE_DHCP 206
#define SUBTYPE_ROUTER 207
#define SUBTYPE_COMBINED_STRING 217

char *mergedLANbeaconCreator (int *argc, char **argv, int *LLDPDU_len) ;
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, char **combinedString, char *source, char *combinedBeacon, int *currentByte);
void transferCombinedBeacon (unsigned char subtype, char *source, char *combinedBeacon, int *currentByte);
void transferCombinedString (char *TLVdescription, char **combinedString, char *source);
void IPparser (int IPv_4or6, char *optarg, char **combinedString, char *myLANbeacon, int *currentByte);
void printHelp();

#endif
