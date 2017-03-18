#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "beacon.h"
#include "tools.h"

/* Howto adding new fields:
	1. Add desired field to structure in Beacon.h
	2. Add desired options in configure.h, then configure.c
	3. Copy field contents from Configure structure to LANbeacon structure in function below
	4. Add field to Fliesstext string
	5. Add fields to size calculation
*/


struct LANbeacon *createLANbeacon(struct LANbeaconProperties *myLANbeaconProperties)
{
	struct LANbeacon *myLANbeacon = malloc(sizeof(struct LANbeacon));
	myLANbeacon->combinedBeacon = malloc(600);		// old code:	malloc(sizeof(char)*(2+myLANbeacon->TLVlength));
	int currentByte = 2;	//counter for current position in Array combinedBeacon, starting after TLV header
	
	//## TLV information string: OUI und subtype ##//
	strncpy(myLANbeacon->TLVorganizationIdentifier,myLANbeaconProperties->organization_identifier,3);
	myLANbeacon->TLVorganizationIdentifier[0] = myLANbeaconProperties->organization_identifier[0] | 0b10000000;	// WARNING: first two bits 11 have to be left like this		REF: nach http://standards.ieee.org/getieee802/download/802-2014.pdf clause 9.3
	myLANbeacon->TLVsubtype = 217;		// Wegen Jahr 2017, beliebig
	// transfer OUI and OUI subtype to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVorganizationIdentifier,3); currentByte += 3;
	myLANbeacon->combinedBeacon[currentByte++] = myLANbeacon->TLVsubtype;
	
	//## TLV VLAN ID and name length ##//
	char *ptr;
	myLANbeacon->VLAN_id = (short int) strtoul(myLANbeaconProperties->VLAN_id,&ptr,10);
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeaconProperties->VLAN_name);
	myLANbeacon->VLAN_name_length = strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME]);
	// transfer VLAN ID + VLAN Name length and VLAN Name to combined Beacon
	unsigned short int VLAN_id_net_byteorder = htons(myLANbeacon->VLAN_id);
	myLANbeacon->combinedBeacon[currentByte++] = ((unsigned char *)(&VLAN_id_net_byteorder))[0];
	myLANbeacon->combinedBeacon[currentByte++] = ((unsigned char *)(&VLAN_id_net_byteorder))[1];
	myLANbeacon->combinedBeacon[currentByte++] = myLANbeacon->VLAN_name_length;
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeacon->VLAN_name_length);
	currentByte += myLANbeacon->VLAN_name_length;
	
	//## Custom string ##//
	strcpy(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT],". ");
	strcat(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT],myLANbeaconProperties->Custom_Text);
	myLANbeacon->custom_String_length = strlen(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT]);
	// transfer custom string to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT],myLANbeacon->custom_String_length);
	currentByte += myLANbeacon->custom_String_length;
	
	//## Combine Information Fliesstext String ##//
	char tempstring[30];
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]," Organizational identifier: ");
	strncat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeaconProperties->organization_identifier,4);
	sprintf(tempstring, " %u. ", myLANbeacon->TLVtype);
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],tempstring);
	
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]," VLAN ID and VLAN Name: ");
	sprintf(tempstring, "%u. ", myLANbeacon->VLAN_id);
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],tempstring);
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME]);	
//	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],);
	
	myLANbeacon->fliesstext_String_length = strlen(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]);
	// transfer fliesstext string to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[currentByte],myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeacon->fliesstext_String_length);
	currentByte += myLANbeacon->fliesstext_String_length;
	
	
	
	//## calculating TLV length without header, then combining TLV Header ##//
	myLANbeacon->TLVlength = 
		3		// sizeof(myLANbeacon->TLVorganizationIdentifier)			// Size: 3
		+ 1		// sizeof(myLANbeacon->TLVsubtype)							// Size: 1
		+ 2		// sizeof(myLANbeacon->VLAN_id)								// Size: 2
		+ 1		// sizeof(myLANbeacon->VLAN_name_length)					// Size: 1
		+ myLANbeacon->VLAN_name_length			// strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME])		// Size: x
		+ myLANbeacon->custom_String_length		// strlen(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT])		// Size: x + 2 extra for added ". "
		+ myLANbeacon->fliesstext_String_length	// TODO	// strlen(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT])	// Size: x
		;
	
//	printf("%i\n",myLANbeacon->TLVlength); // DEBUG
	
	myLANbeacon->TLVtype = 127;
	myLANbeacon->TLVheader_combined = (myLANbeacon->TLVtype * 0b1000000000) | myLANbeacon->TLVlength;	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit
	// transfer combined TLV Header to combined Beacon
//	unsigned short int TLVheader_combined_net_byteorder = htons(myLANbeacon->TLVheader_combined);
	myLANbeacon->combinedBeacon[0] = ((unsigned char *)(&myLANbeacon->TLVheader_combined))[1];
	myLANbeacon->combinedBeacon[1] = ((unsigned char *)(&myLANbeacon->TLVheader_combined))[0];
	
	
	puts ("LANbeacon combined. Result: \n");
	FILE *combined = fopen("combinedBeacon","w");
	fwrite(myLANbeacon->combinedBeacon, sizeof(char)*(2+myLANbeacon->TLVlength), 1, combined);		// Two additional bytes for header (TLVtype and TLVlength)
	
	return myLANbeacon;
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
	
	BEACON_VAR_PRINT(myLANbeacon.fliesstext_String_length);
	
	printf("Size of LANbeacon: %zu\n\n",sizeof(myLANbeacon));
//	BEACON_VAR_PRINT(myLANbeacon);
}
