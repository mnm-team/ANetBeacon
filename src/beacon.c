#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "beacon.h"
#include "tools.h"

struct LANbeacon *createLANbeacon(struct LANbeaconProperties *myLANbeaconProperties)
{
	struct LANbeacon *myLANbeacon = malloc(sizeof(struct LANbeacon));
	
	// TLV information string: OUI und subtype
	strncpy(myLANbeacon->TLVorganizationIdentifier,myLANbeaconProperties->organization_identifier,3);
	myLANbeacon->TLVorganizationIdentifier[0] = myLANbeaconProperties->organization_identifier[0] | 0b10000000;
	
//	myLANbeacon->TLVorganizationIdentifier[0] = myLANbeaconProperties->organization_identifier[0] | 0b10000000;
//	myLANbeacon->TLVorganizationIdentifier[1] = myLANbeaconProperties->organization_identifier[1];
//	myLANbeacon->TLVorganizationIdentifier[2] = myLANbeaconProperties->organization_identifier[2];
	myLANbeacon->TLVsubtype = 217;		// Wegen Jahr 2017
	
	// TLV VLAN ID and name length
	char *ptr;
	myLANbeacon->VLAN_id = (short int) strtoul(myLANbeaconProperties->VLAN_id,&ptr,10);
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeaconProperties->VLAN_name);
	myLANbeacon->VLAN_name_length = strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME]);
	
	// Custom string
	strcpy(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT],myLANbeaconProperties->Custom_Text);
	myLANbeacon->custom_String_length = strlen(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT]);
	
	// Information string
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],"Das ist ein Fliesstext-Test");
	myLANbeacon->fliesstext_String_length = strlen(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]);
	
	// TLV length without header
	myLANbeacon->TLVlength = getBeaconLength(myLANbeacon);
	// TLV Header:
	myLANbeacon->TLVtype = 127;
	myLANbeacon->TLVheader_combined = (myLANbeacon->TLVtype * 0b1000000000) | myLANbeacon->TLVlength;	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit
	
	return myLANbeacon;
}


void combineBeacon(struct LANbeacon *myLANbeacon)	// TODO
{
	myLANbeacon->combinedBeacon = malloc(sizeof(char)*(2+myLANbeacon->TLVlength));
	int currentByte = 0;	//counter for current position in Array combinedBeacon
	
	// transfer combined TLV Header to combined Beacon
//	unsigned short int TLVheader_combined_net_byteorder = htons(myLANbeacon->TLVheader_combined);
	myLANbeacon->combinedBeacon[currentByte++] = ((unsigned char *)(&myLANbeacon->TLVheader_combined))[1];
	myLANbeacon->combinedBeacon[currentByte++] = ((unsigned char *)(&myLANbeacon->TLVheader_combined))[0];
	
	// transfer OUI and OUI subtype to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVorganizationIdentifier,3); currentByte +=3;
	myLANbeacon->combinedBeacon[currentByte++] = myLANbeacon->TLVsubtype;
	
	// transfer VLAN ID + VLAN Name length and VLAN Name to combined Beacon
	unsigned short int VLAN_id_net_byteorder = htons(myLANbeacon->VLAN_id);
	myLANbeacon->combinedBeacon[currentByte++] = ((unsigned char *)(&VLAN_id_net_byteorder))[0];
	myLANbeacon->combinedBeacon[currentByte++] = ((unsigned char *)(&VLAN_id_net_byteorder))[1];
	
	myLANbeacon->combinedBeacon[currentByte++] = myLANbeacon->VLAN_name_length;
	
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeacon->VLAN_name_length);
	currentByte += myLANbeacon->VLAN_name_length;
	
	// transfer custom string to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT],myLANbeacon->custom_String_length);
	currentByte += myLANbeacon->custom_String_length;
	
	// transfer fliesstext string to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeacon->fliesstext_String_length);
	currentByte += myLANbeacon->fliesstext_String_length;
	
	
	
	
	
	
	
	puts ("LANbeacon combined. Result: \n");
	FILE *combined = fopen("combinedBeacon","w");
	fwrite(myLANbeacon->combinedBeacon, sizeof(char)*(2+myLANbeacon->TLVlength), 1, combined);
}


unsigned short int getBeaconLength (struct LANbeacon *myLANbeacon)
{
	return
		  sizeof(myLANbeacon->TLVorganizationIdentifier)			// Size: 3
		+ sizeof(myLANbeacon->TLVsubtype)							// Size: 1
		+ sizeof(myLANbeacon->VLAN_id)								// Size: 2
		+ sizeof(myLANbeacon->VLAN_name_length)						// Size: 1
		+ strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME])		// Size: x
		+ strlen(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT])		// Size: x
		+ strlen(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT])	// Size: x
		;
}


#define BEACON_VAR_PRINT(aktuellesElement) puts(#aktuellesElement ":"); printVarInFormats(sizeof(aktuellesElement),&aktuellesElement);

#define BEACON_STR_PRINT(aktuellesElement) puts(#aktuellesElement ":"); printf("String:\t%s\n",aktuellesElement); printVarInFormats(sizeof(aktuellesElement),aktuellesElement); 

void printLANbeacon(struct LANbeacon myLANbeacon)
{
	FILE *binBeacon = fopen("binBeacon","w");
	fwrite(&myLANbeacon, sizeof(struct LANbeacon), 1, binBeacon);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVtype);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVlength);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVheader_combined);
	
	BEACON_STR_PRINT(myLANbeacon.TLVorganizationIdentifier);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVsubtype);
	
	BEACON_VAR_PRINT(myLANbeacon.VLAN_id);
	
	BEACON_VAR_PRINT(myLANbeacon.VLAN_name_length);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVsubtype);
	
	BEACON_STR_PRINT(myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME]);
	
	BEACON_STR_PRINT(myLANbeacon.TLVinformationString[TLV_INFO_FLIESSTEXT]);
	
	printf("Size of LANbeacon: %zu\n\n",sizeof(myLANbeacon));
//	BEACON_VAR_PRINT(myLANbeacon);
	
}
