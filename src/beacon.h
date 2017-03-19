#ifndef BEACON_H
#define BEACON_H

#define TLV_INFO_STRINGS_BUF_SIZE 100
#define TLV_INFO_STRINGS_NUMBER 10
#define ORGANIZATIONAL_IDENTIFIER 0
#define TLV_INFO_VLAN_NAME 1
#define VLAN_ID 2
#define TLV_CUSTOM_TEXT 3
#define TLV_INFO_FLIESSTEXT 4
#define IPV4_NETWORKS 5
#define IPV6_NETWORKS 6
#define CONTACT_EMAIL 7
#define DHCP_TYPES 8
#define ROUTER_INFO 9
#include "config.h"

struct LANbeacon {
//struct LANbeacon {
	//	TLV Header:
	unsigned char TLVtype;
	unsigned short int TLVlength;
	unsigned short int TLVheader_combined;
	
	// TLV information string: OUI und subtype
	char TLVorganizationIdentifier[3];	// WARNING: first two bits 11 have to be left like this		REF: nach http://standards.ieee.org/getieee802/download/802-2014.pdf clause 9.3
	unsigned char TLVsubtype;
	
	// TLV VLAN ID and name length
	unsigned short int VLAN_id;
	unsigned char VLAN_name_length;		// max. 32 octets
	
	// TLV information string:	"Payload"
	char TLVinformationString[TLV_INFO_STRINGS_NUMBER][TLV_INFO_STRINGS_BUF_SIZE];
	unsigned char stringLengths[TLV_INFO_STRINGS_NUMBER];
	
	// Beacon after combination:
	char *combinedBeacon;
	int currentByte;
};

void copyStringToCombinedBeacon (int currentString, struct LANbeacon *myLANbeacon, struct LANbeaconProperties *myLANbeaconProperties);
struct LANbeacon *createLANbeacon(struct LANbeaconProperties *myLANbeaconProperties);
void puttogetherLANbeacon (struct LANbeacon myLANbeacon);
// unsigned short int getBeaconLength (struct LANbeaconProperties *myLANbeaconProperties, struct LANbeacon *myLANbeacon);

#endif

