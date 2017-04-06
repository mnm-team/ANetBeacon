#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H

char *mergedLANbeaconCreator (int *argc, char **argv, int *LLDPDU_len) ;
void StringTransfer (char *target, char *origin, int maxsize);
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, char **combinedString, char *source, char *combinedBeacon, int *currentByte);
void transferCombinedBeacon (unsigned char subtype, char *source, char *combinedBeacon, int *currentByte);
void transferCombinedString (char *TLVdescription, char **combinedString, char *source);
void IPparser (int IPv_4or6, char *optarg, char **combinedString, char *myLANbeacon, int *currentByte);
void printHelp();

#endif
