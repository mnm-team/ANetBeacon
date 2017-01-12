
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"


int main(int argc, char **argv) {
	
	struct LANbeacon myLANbeacon = createLANbeacon(); 
	
	
	
//	printVarInFormats(sizeof(myLANbeacon.TLVtype),&myLANbeacon.TLVtype);
	
	
	printVarInFormats(sizeof(myLANbeacon.TLVorganizationIdentifier),myLANbeacon.TLVorganizationIdentifier);
	
	
	// ###### SPIELWIESE ######
	printf("\n\n##########\nSPIELWIESE\n##########\n");
	
	
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









