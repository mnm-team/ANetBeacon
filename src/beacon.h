#ifndef BEACON_H
#define BEACON_H

struct LANbeacon {
	char TLVtype;
	unsigned int TLVlength;		// TODO: bezieht sich Länge nur auf Payload oder was alles?
	char TLVorganizationIdentifier[4];
	char TLVsubtype;
};

struct LANbeacon createLANbeacon();

#endif

