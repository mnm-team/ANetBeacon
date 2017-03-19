#ifndef CONFIG_H
#define CONFIG_H

struct LANbeaconProperties {
	
	char TLVpropertyStrings[TLV_INFO_STRINGS_NUMBER][TLV_INFO_STRINGS_BUF_SIZE];
	
};


struct LANbeaconProperties *setLANbeaconProperties(int *argc, char **argv);
void StringTransfer (char *target, char *origin, int maxsize);
void printHelp();


#endif

