#ifndef BEACON_H
#define BEACON_H

#define TLV_INFO_STRINGS_BUF_SIZE 36
#define TLV_INFO_STRINGS_NUMBER 3
#define TLV_INFO_VLAN_NAME 0
#define TLV_CUSTOM_TEXT 1
#define TLV_INFO_FLIESSTEXT 2
#include "config.h"


struct __attribute__((__packed__)) LANbeacon {
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
};

struct LANbeacon *createLANbeacon(struct LANbeaconProperties *myLANbeaconProperties);
void printLANbeacon(struct LANbeacon myLANbeacon);
void puttogetherLANbeacon (struct LANbeacon myLANbeacon);
unsigned short int getBeaconLength (struct LANbeacon *myLANbeacon);
void combineBeacon(struct LANbeacon myLANbeacon);

#endif

