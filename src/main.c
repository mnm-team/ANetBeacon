#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"

int main(int argc, char **argv) {
	
	struct LANbeaconProperties *myLANbeaconProperties = setLANbeaconProperties(&argc, argv);
	
	struct LANbeacon myLANbeacon = createLANbeacon(); 
	printLANbeacon(myLANbeacon);
	combineBeacon(myLANbeacon);
	
	
	
	// ###### SPIELWIESE ######
	printf("\n\n##########\nSPIELWIESE\n##########\n");
	
	unsigned char testblabla[4];
	unsigned char yolopolo;
	
	yolopolo = 0b11110101;
	
//	*testblabla = 0b111101010111101100010010;
	
	for (int i = 0;i<4;i++)
	{
		printf("%c ",myLANbeacon.TLVorganizationIdentifier[i]);
		printf("\t%p\n",&myLANbeacon.TLVorganizationIdentifier[i]);
	}
	
	unsigned char *TLVlengthPointer = (unsigned char*) &myLANbeacon.TLVlength;
	for (int i = 0;i<4;i++)
	{
		binPrint(sizeof(*TLVlengthPointer),TLVlengthPointer);
		printf("\t%p\n",TLVlengthPointer++);
	}
	
	
	return 0;
}
