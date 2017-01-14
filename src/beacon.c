#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"

struct LANbeacon createLANbeacon()
{
	struct LANbeacon myLANbeacon;
	
	//	TLV Header:
	myLANbeacon.TLVtype = 127;
	myLANbeacon.TLVlength = 0b0000000101010101; // TODO Arbitrary value for testing
	myLANbeacon.TLVheader_combined = (myLANbeacon.TLVtype * 0b1000000000) | myLANbeacon.TLVlength;	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit
	
	// TLV information string: OUI und subtype
	myLANbeacon.TLVorganizationIdentifier[0] = 0b11001100;	//	0b111101010111101100010010
	myLANbeacon.TLVorganizationIdentifier[1] = 0b11111111;
	myLANbeacon.TLVorganizationIdentifier[2] = 0b11111111;
	myLANbeacon.TLVsubtype = 217;		// Wegen Jahr 2017
	
	// TLV VLAN ID and name length
	myLANbeacon.VLAN_id = 0b110010100011;
	strcpy(myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME],"LMU IFI Test-VLAN");
	myLANbeacon.VLAN_name_plus_id_length = strlen(myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME]);
	
	// Information string
	strcpy(myLANbeacon.TLVinformationString[TLV_INFO_FLIESSTEXT],"Das ist ein Infostring-Test");
	
	return myLANbeacon;
}


void printLANbeacon(struct LANbeacon myLANbeacon)
{
	puts("myLANbeacon.TLVtype:");
	printVarInFormats(sizeof(myLANbeacon.TLVtype),&myLANbeacon.TLVtype);	
	
	puts("myLANbeacon.TLVlength:");
	printVarInFormats(sizeof(myLANbeacon.TLVlength),&myLANbeacon.TLVlength);	
	
	puts("myLANbeacon.TLVheader_combined:");
	printVarInFormats(sizeof(myLANbeacon.TLVheader_combined),&myLANbeacon.TLVheader_combined);	
	
	puts("myLANbeacon.TLVorganizationIdentifier:");
	printVarInFormats(sizeof(myLANbeacon.TLVorganizationIdentifier),myLANbeacon.TLVorganizationIdentifier);	
	
	puts("myLANbeacon.TLVsubtype:");
	printVarInFormats(sizeof(myLANbeacon.TLVsubtype),&myLANbeacon.TLVsubtype);
	
	puts("myLANbeacon.VLAN_id:");
	printVarInFormats(sizeof(myLANbeacon.VLAN_id),&myLANbeacon.VLAN_id);
	
	puts("myLANbeacon.VLAN_name_plus_id_length:");
	printVarInFormats(sizeof(myLANbeacon.VLAN_name_plus_id_length),&myLANbeacon.VLAN_name_plus_id_length);
	
	puts("myLANbeacon.TLVsubtype:");
	printVarInFormats(sizeof(myLANbeacon.VLAN_name_plus_id_length),&myLANbeacon.VLAN_name_plus_id_length);
	
	puts("myLANbeacon.myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME]:");
	printVarInFormats(sizeof(myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME]),myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME]);
	
	puts("myLANbeacon.myLANbeacon.TLVinformationString[TLV_INFO_FLIESSTEXT]:");
	printVarInFormats(sizeof(myLANbeacon.TLVinformationString[TLV_INFO_FLIESSTEXT]),myLANbeacon.TLVinformationString[TLV_INFO_FLIESSTEXT]);
	
	printf("Size of LANbeacon: %zu\n\n",sizeof(myLANbeacon));
	puts("myLANbeacon:");
	printVarInFormats(sizeof(myLANbeacon),&myLANbeacon);	
	
}


unsigned short int getBeaconLength (struct LANbeacon myLANbeacon)
{
	printf("test");
	
	return 13;
}

