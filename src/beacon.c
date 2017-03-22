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
	myLANbeacon->combinedBeacon = malloc(1500);		// old code:	malloc(sizeof(char)*(2+myLANbeacon->TLVlength));
	myLANbeacon->currentByte = 2;	//counter for current position in Array combinedBeacon, starting after TLV header
	
	//## TLV type, is inserted into combined LANbeacon later ##//
	myLANbeacon->TLVtype = 127;
	
	//## TLV information string: OUI und subtype ##//
	strncpy(myLANbeacon->TLVorganizationIdentifier,myLANbeaconProperties->TLVpropertyStrings[ORGANIZATIONAL_IDENTIFIER],3);
	myLANbeacon->TLVorganizationIdentifier[0] = myLANbeaconProperties->TLVpropertyStrings[ORGANIZATIONAL_IDENTIFIER][0] | 0b10000000;	// WARNING: first two bits 11 have to be left like this		REF: nach http://standards.ieee.org/getieee802/download/802-2014.pdf clause 9.3
	myLANbeacon->TLVsubtype = 217;		// Wegen Jahr 2017, beliebig
	// transfer OUI and OUI subtype to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[myLANbeacon->currentByte],myLANbeacon->TLVorganizationIdentifier,3); 
	myLANbeacon->currentByte += 3;
	myLANbeacon->combinedBeacon[myLANbeacon->currentByte++] = myLANbeacon->TLVsubtype;
	
	//## TLV VLAN ID and name length ##//
	char *ptr;
	myLANbeacon->VLAN_id = (short int) strtoul(myLANbeaconProperties->TLVpropertyStrings[VLAN_ID],&ptr,10);
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeaconProperties->TLVpropertyStrings[TLV_INFO_VLAN_NAME]);
	myLANbeacon->VLAN_name_length = strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME]);
	// transfer VLAN ID + VLAN Name length and VLAN Name to combined Beacon
	unsigned short int VLAN_id_net_byteorder = htons(myLANbeacon->VLAN_id);
	myLANbeacon->combinedBeacon[myLANbeacon->currentByte++] = ((unsigned char *)(&VLAN_id_net_byteorder))[0];
	myLANbeacon->combinedBeacon[myLANbeacon->currentByte++] = ((unsigned char *)(&VLAN_id_net_byteorder))[1];
	myLANbeacon->combinedBeacon[myLANbeacon->currentByte++] = myLANbeacon->VLAN_name_length;
	strncpy(&myLANbeacon->combinedBeacon[myLANbeacon->currentByte],myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME],myLANbeacon->VLAN_name_length);
	myLANbeacon->currentByte += myLANbeacon->VLAN_name_length;
	
	
	copyStringToCombinedBeacon (IPV4_NETWORKS, myLANbeacon, myLANbeaconProperties);
	copyStringToCombinedBeacon (IPV6_NETWORKS, myLANbeacon, myLANbeaconProperties);
	copyStringToCombinedBeacon (CONTACT_EMAIL, myLANbeacon, myLANbeaconProperties);
	copyStringToCombinedBeacon (DHCP_TYPES, myLANbeacon, myLANbeaconProperties);
	copyStringToCombinedBeacon (ROUTER_INFO, myLANbeacon, myLANbeaconProperties);
	copyStringToCombinedBeacon (TLV_CUSTOM_TEXT, myLANbeacon, myLANbeaconProperties);
	
	//## Combine Information Fliesstext String ##//
	char tempstring[30];
	strcpy(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]," Organizational identifier: ");
	strncat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeaconProperties->TLVpropertyStrings[ORGANIZATIONAL_IDENTIFIER],4);
	sprintf(tempstring, " %u. ", myLANbeacon->TLVtype);
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],tempstring);
	
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]," VLAN ID and VLAN Name: ");
	sprintf(tempstring, "%u. ", myLANbeacon->VLAN_id);
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],tempstring);
	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME]);	
//	strcat(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],);
	
	myLANbeacon->stringLengths[TLV_INFO_FLIESSTEXT] = strlen(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT]);
	// transfer fliesstext string to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[myLANbeacon->currentByte],myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT],myLANbeacon->stringLengths[TLV_INFO_FLIESSTEXT]);
	myLANbeacon->currentByte += myLANbeacon->stringLengths[TLV_INFO_FLIESSTEXT];
	
	
	
	
	
	//## calculating TLV length without header, then combining TLV Header ##//
	myLANbeacon->TLVlength = myLANbeacon->currentByte;
	if (myLANbeacon->currentByte > 505) {printf("Warning, TLV is %i Bytes long, exceeding maximum of 505 characters. End of string will be cut off.", myLANbeacon->currentByte );	myLANbeacon->TLVlength = 505;}	// TODO Maximale Größe überprüfen
	myLANbeacon->TLVheader_combined = (myLANbeacon->TLVtype * 0b1000000000) | (myLANbeacon->TLVlength-2);	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit
	// transfer combined TLV Header to combined Beacon	// DEPRECATED: unsigned short int TLVheader_combined_net_byteorder = htons(myLANbeacon->TLVheader_combined);
	myLANbeacon->combinedBeacon[0] = ((unsigned char *)(&myLANbeacon->TLVheader_combined))[1];
	myLANbeacon->combinedBeacon[1] = ((unsigned char *)(&myLANbeacon->TLVheader_combined))[0];
	
	
	puts ("LANbeacon combined. Result: \n");
	FILE *combined = fopen("combinedBeacon","w");
	fwrite(myLANbeacon->combinedBeacon, sizeof(char)*(myLANbeacon->TLVlength), 1, combined);		// Two additional bytes for header (TLVtype and TLVlength)
	
	return myLANbeacon;
}

void copyStringToCombinedBeacon (int currentString, struct LANbeacon *myLANbeacon, struct LANbeaconProperties *myLANbeaconProperties) {
	strcpy(myLANbeacon->TLVinformationString[currentString],". ");
	strcat(myLANbeacon->TLVinformationString[currentString],myLANbeaconProperties->TLVpropertyStrings[currentString]);
	myLANbeacon->stringLengths[currentString] = strlen(myLANbeacon->TLVinformationString[currentString]);
	
	// transfer string to combined Beacon
	strncpy(&myLANbeacon->combinedBeacon[myLANbeacon->currentByte],myLANbeacon->TLVinformationString[currentString],myLANbeacon->stringLengths[currentString]);
	myLANbeacon->currentByte += myLANbeacon->stringLengths[currentString];
}

/*		DEPRECATED CODE
		3		// sizeof(myLANbeacon->TLVorganizationIdentifier)			// Size: 3
		+ 1		// sizeof(myLANbeacon->TLVsubtype)							// Size: 1
		+ 2		// sizeof(myLANbeacon->VLAN_id)								// Size: 2
		+ 1		// sizeof(myLANbeacon->VLAN_name_length)					// Size: 1
		+ myLANbeacon->VLAN_name_length			// strlen(myLANbeacon->TLVinformationString[TLV_INFO_VLAN_NAME])		// Size: x
		+ myLANbeacon->custom_String_length		// strlen(myLANbeacon->TLVinformationString[TLV_CUSTOM_TEXT])		// Size: x + 2 extra for added ". "
		+ myLANbeacon->fliesstext_String_length	// TODO	// strlen(myLANbeacon->TLVinformationString[TLV_INFO_FLIESSTEXT])	// Size: x
		;	
	printf("current Byte: %i ; length: %i \n",currentByte,myLANbeacon->TLVlength);	// DEBUG
*/
