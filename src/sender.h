#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H

char *mergedLANbeaconCreator (int *argc, char **argv, int *lldpdu_len) ;
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, char **combinedString, char *source, char *combinedBeacon, int *currentByte);
void transferToCombinedBeacon (unsigned char subtype, char *source, char *combinedBeacon, int *currentByte);
void transferToCombinedString (char *TLVdescription, char **combinedString, char *source);
void ipParser (int ip_V4or6, char *optarg, char **combinedString, char *myLANbeacon, int *currentByte);
void printHelp();

#endif
