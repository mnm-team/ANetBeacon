#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"

struct LANbeaconProperties *setLANbeaconProperties(int *argc, char **argv) {		// TODO
	

//	char *configFile;
	
	struct LANbeaconProperties *myLANbeaconProperties = malloc(sizeof(struct LANbeaconProperties));

//	configFile = malloc(sizeof(char)*INPUT_STRINGS_SIZE);
//	strncpy(configFile, CONFIGFILE, INPUT_STRINGS_SIZE-1);

	if(*argc == 1) printHelp();
	
	int opt;
	while((opt=getopt(*argc, argv, "i:n:c:h")) != -1) {
		switch(opt) {
			case 'i':
				strncpy(myLANbeaconProperties->VLAN_id, optarg, INPUT_STRINGS_SIZE-1);	// TODO Überprüfung der Zahl
				break;
				
			case 'n':
				strncpy(myLANbeaconProperties->VLAN_name, optarg, INPUT_STRINGS_SIZE-1);
				if(strlen(myLANbeaconProperties->VLAN_name) > 32)
				{
					puts("Error because the VLAN name string is longer than 32 Digits.");
					exit(EXIT_FAILURE);
				}
				break;
				
			case 'c':
				strncpy(myLANbeaconProperties->Custom_Text, optarg, INPUT_STRINGS_SIZE-1);
				if(strlen(myLANbeaconProperties->Custom_Text) > TLV_INFO_STRINGS_BUF_SIZE)
				{
					printf("Error because the custom string is longer than %i Digits.\n", TLV_INFO_STRINGS_BUF_SIZE);
					exit(EXIT_FAILURE);
				}
				break;

			// TODO: alte rausschmeißen:
/*			case 'c':
				strncpy(configFile, optarg, INPUT_STRINGS_SIZE-1);
				break;
			case 'd':
				myLANbeaconProperties->debugmode = 1;
				break;
*/			case 'h':
				printHelp();
				break;
			default:
				printHelp();
		}
	}

//	readConfigFile(myLANbeaconProperties, configFile);

	return myLANbeaconProperties;
}


void printHelp() {		// TODO
	printf("Usage: \t./client [-i GAMEID] [-c CONFIGFILE]\n");
	printf("\t./client -h\n");
	exit(EXIT_FAILURE);
}

