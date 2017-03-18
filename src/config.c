#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"

// code based on https://github.com/ciil/nine-mens-morris/blob/master/src/config.c
struct LANbeaconProperties *setLANbeaconProperties(int *argc, char **argv) {
	
	struct LANbeaconProperties *myLANbeaconProperties = malloc(sizeof(struct LANbeaconProperties));
	
	// Setting default values
	strcpy(myLANbeaconProperties->VLAN_name, "");
	strcpy(myLANbeaconProperties->Custom_Text, "");
	strcpy(myLANbeaconProperties->VLAN_id, "101");
	strcpy(myLANbeaconProperties->organization_identifier, "LMU");

	if(*argc == 1) printHelp();
	int opt;
	while((opt=getopt(*argc, argv, "i:n:c:o:h")) != -1) {
		switch(opt) {
			case 'i':
				strncpy(myLANbeaconProperties->VLAN_id, optarg, INPUT_STRINGS_SIZE-1);	// TODO Überprüfung der Zahl
				break;
				
			case 'o':
				strncpy(myLANbeaconProperties->organization_identifier, optarg, INPUT_STRINGS_SIZE-1);	// TODO Überprüfung der Anzahl
				break;
				
			case 'n':
				strncpy(myLANbeaconProperties->VLAN_name, optarg, INPUT_STRINGS_SIZE-1);
				if(strlen(myLANbeaconProperties->VLAN_name) > 32)	// TODO Überprüfung
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

			case 'h':
				printHelp();
				break;
				
			default:
				printHelp();
		}
	}

	return myLANbeaconProperties;
}


void printHelp() {
	printf("Usage: \t./LANbeacon [-i VLAN_ID] [-o ORGANIZATIONAL_IDENTIFIER] [-n VLAN_NAME] [-c CUSTOM_STRING]\n");
	printf("\t./client -h\n");
	exit(EXIT_FAILURE);
}

