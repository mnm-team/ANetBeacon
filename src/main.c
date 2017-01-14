#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"

int main(int argc, char **argv) {
	
	struct LANbeacon myLANbeacon = createLANbeacon(); 
	
	printLANbeacon(myLANbeacon);
	
	
	
	
	
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
	
//	printf("%zu\n", sizeof(short unsigned int));
	
	short unsigned int TLVheaderTest = 0b00000000;		// header of the custom TLV; used for TLV Type and Length
	
	// char yolo = 127;
	// printVarInFormats(sizeof(yolo),&yolo);
	
	TLVheaderTest = TLVheaderTest | 0b0101010101010101;
	
	// printVarInFormats(sizeof(TLVheaderTest),&TLVheaderTest);
	
	int ret;
	while ((ret=getopt(argc, argv, "f:t:v:")) != -1) {
		switch (ret) {
			case 'f':
				optarg = optarg;
				break;
			default:
				printf("Bitte Parameter angeben.\n");
				break;
		}
	}
	
	
	return 0;
}
