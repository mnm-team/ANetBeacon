#ifndef BEACON_H
#define BEACON_H

struct LANbeacon {
	//	TLV Header:
	unsigned char TLVtype;
	unsigned short int TLVlength;		// TODO: bezieht sich Länge nur auf Payload oder was alles?
	
	// TLV information string:	Identifier
	char TLVorganizationIdentifier[4];
	unsigned char TLVsubtype;
	
	//TLV information string:	"Payload"
		// TODO: Array von Char-Arrays
};

struct LANbeacon createLANbeacon();
void printLANbeacon(struct LANbeacon myLANbeacon);
void puttogetherLANbeacon (struct LANbeacon myLANbeacon);

#endif

