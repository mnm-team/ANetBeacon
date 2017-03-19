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
	for (int n=TLV_INFO_STRINGS_NUMBER; n>0; n--) {strcpy(myLANbeaconProperties->TLVpropertyStrings[n],"");}
	strcpy(myLANbeaconProperties->TLVpropertyStrings[VLAN_ID], "101");
	strcpy(myLANbeaconProperties->TLVpropertyStrings[ORGANIZATIONAL_IDENTIFIER], "LMU");
	
	if(*argc == 1) printHelp();
	int opt;
	while((opt=getopt(*argc, argv, "i:o:n:c:4:6:e:d:r:h")) != -1) {
		switch(opt) {
			case 'i':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[VLAN_ID], optarg, 5);	// TODO Überprüfung der Zahl
				break;
				
			case 'o':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[ORGANIZATIONAL_IDENTIFIER], optarg, 3);
				break;
				
			case 'n':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[TLV_INFO_VLAN_NAME], optarg, 32);
				break;
				
			case 'c':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[TLV_CUSTOM_TEXT], optarg, TLV_INFO_STRINGS_BUF_SIZE);
				break;
				
			case '4':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[IPV4_NETWORKS], optarg, 20);
				break;

			case '6':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[IPV6_NETWORKS], optarg, 50);
				break;

			case 'e':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[CONTACT_EMAIL], optarg, 50);
				break;

			case 'd':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[DHCP_TYPES], optarg, TLV_INFO_STRINGS_BUF_SIZE);
				break;

			case 'r':
				StringTransfer(myLANbeaconProperties->TLVpropertyStrings[ROUTER_INFO], optarg, TLV_INFO_STRINGS_BUF_SIZE);
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

void StringTransfer (char *target, char *origin, int maxsize) {
	if(strlen(origin) > maxsize) {
		printf("Error because string length limit of %i digits exceeded.\n", maxsize);
		exit(EXIT_FAILURE);
	}
	strncpy(target, origin, maxsize-1);
}

void printHelp() {
	printf("Usage: \t./LANbeacon [-o ORGANIZATIONAL_IDENTIFIER] [-i VLAN_ID] [-n VLAN_NAME] [-4 IPv4_NETWORKS] [-6 IPv6_NETWORKS] [-e EMAIL_CONTACTPERSON] [-d DHCP_TYPES] [-r ROUTER_INFORMATION] [-c CUSTOM_STRING]\n");
	printf("\t./client -h\n");
	exit(EXIT_FAILURE);
}

