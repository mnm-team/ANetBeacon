#ifndef MERGEDBEACON_H
#define MERGEDBEACON_H

char *mergedLANbeaconCreator (int *argc, char **argv) ;
void StringTransfer (char *target, char *origin, int maxsize);
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, char **combinedString, char *source, char *combinedBeacon, int *currentByte);
void transferCombinedBeacon (unsigned char subtype, char *source, char *combinedBeacon, int *currentByte);
void transferCombinedString (char *TLVdescription, char **combinedString, char *source);
void printHelp();

#endif
