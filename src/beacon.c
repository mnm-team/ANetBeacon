#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"


struct LANbeacon createLANbeacon()
{
	struct LANbeacon myLANbeacon;
	
	// Hier wird der LANBeacon befüllt
	myLANbeacon.TLVtype = 127;
	strcpy(myLANbeacon.TLVorganizationIdentifier,"LMU");
	myLANbeacon.TLVsubtype = 217;		// Wegen Jahr 2017
	
	myLANbeacon.TLVlength = 0b0000000101010101; // 0b11001100101010100101010100110011; // TODO Arbitrary value for testing
	
	return myLANbeacon;
	
}


void printLANbeacon(struct LANbeacon myLANbeacon)
{
	printf("Size of LANbeacon: %zu\n\n",sizeof(myLANbeacon));
	
	puts("myLANbeacon.TLVtype:");
	printVarInFormats(sizeof(myLANbeacon.TLVtype),&myLANbeacon.TLVtype);	
	
	puts("myLANbeacon.TLVlength:");
	printVarInFormats(sizeof(myLANbeacon.TLVlength),&myLANbeacon.TLVlength);	
	
	puts("myLANbeacon.TLVorganizationIdentifier:");
	printVarInFormats(sizeof(myLANbeacon.TLVorganizationIdentifier),myLANbeacon.TLVorganizationIdentifier);	
	
	puts("myLANbeacon.TLVsubtype:");
	printVarInFormats(sizeof(myLANbeacon.TLVsubtype),&myLANbeacon.TLVsubtype);	
	
	puts("myLANbeacon:");
	printVarInFormats(sizeof(myLANbeacon),&myLANbeacon);	
	
}


void puttogetherLANbeacon (struct LANbeacon myLANbeacon)
{
	
}






