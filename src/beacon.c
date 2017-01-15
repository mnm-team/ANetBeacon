#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "beacon.h"
#include "tools.h"

struct LANbeacon *createLANbeacon(struct LANbeaconProperties *myLANbeaconProperties)
{
	struct LANbeacon *myLANbeacon = malloc(sizeof(struct LANbeacon));
	
	// TLV information string: OUI und subtype
	myLANbeacon->TLVorganizationIdentifier[0] = myLANbeaconProperties->organization_identifier[0] | 0b10000000;
	myLANbeacon->TLVorganizationIdentifier[1] = myLANbeaconProperties->organization_identifier[1];
	myLANbeacon->TLVorganizationIdentifier[2] = myLANbeaconProperties->organization_identifier[2];
	myLANbeacon->TLVsubtype = 217;		// Wegen Jahr 2017
	
	// TLV VLAN ID and name length
	char *ptr;
	myLANbeacon->VLAN_id = (short int) strtoul(myLANbeaconProperties->VLAN_id,&ptr,10);
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeaconProperties->VLAN_name);
	myLANbeacon->VLAN_name_length = strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME]);
	
	// Custom string
	strcpy(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT],myLANbeaconProperties->Custom_Text);
	
	// Information string
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],"Das ist ein Fließtext-Test");
	
	// TLV length without header
	myLANbeacon->TLVlength = getBeaconLength(myLANbeacon);
	// TLV Header:
	myLANbeacon->TLVtype = 127;
	myLANbeacon->TLVheader_combined = (myLANbeacon->TLVtype * 0b1000000000) | myLANbeacon->TLVlength;	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit
	
	return myLANbeacon;
}


unsigned short int getBeaconLength (struct LANbeacon *myLANbeacon)
{
	return
		  sizeof(myLANbeacon->TLVorganizationIdentifier)			// Size: 3
		+ sizeof(myLANbeacon->TLVsubtype)							// Size: 1
		+ sizeof(myLANbeacon->VLAN_id)								// Size: 2
		+ sizeof(myLANbeacon->VLAN_name_length)						// Size: 1
		+ strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME])	// Size: x
		+ strlen(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT])	// Size: x
		;
}


void combineBeacon(struct LANbeacon myLANbeacon)	// TODO
{
	
	puts ("LANbeacon being combined.");
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
