
#include <stdio.h>
#include <unistd.h>
#include "tools.h"


// code based on http://stackoverflow.com/a/3974138
void printVarInFormats(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;
	int i, j;
	
	unsigned short int *myInt = (unsigned short int*) ptr;
	
	//Start printing values
	printf("DEC:\t%d\n", *myInt);
	printf("sint DEC:\t%hu\n", *myInt);
	printf("char DEC:\t%hu\n", *b);
	
	printf("Binary:\t");
	for (i=size-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
		printf(" ");
		
		if ((i>0)&&((size-i)%4 == 0)) {printf("\n\t");}		//Newline each 4 blocks
	}
	
	// code based on http://stackoverflow.com/a/12725151
	printf("\nHEX:\t");
	for (i=size-1;i>=0;i--){
		printf("%02x ", (unsigned int) *b++);
		if ((i>0)&&((size-i)%4 == 0)) {printf("\n\t");}		//Newline each 4 blocks
	}
	
	puts("\n");
	
}


#define BEACON_VAR_PRINT(aktuellesElement) puts(#aktuellesElement ":"); printVarInFormats(sizeof(aktuellesElement),&aktuellesElement);
#define BEACON_STR_PRINT(aktuellesElement) puts(#aktuellesElement ":"); printf("String:\t%s\n",aktuellesElement); printVarInFormats(sizeof(aktuellesElement),aktuellesElement); 
void printLANbeacon(struct LANbeacon myLANbeacon)
{
	FILE *binBeacon = fopen("binBeacon","w");
	fwrite(&myLANbeacon, sizeof(struct LANbeacon), 1, binBeacon);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVtype);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVlength);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVheader_combined);
	
	BEACON_STR_PRINT(myLANbeacon.TLVorganizationIdentifier);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVsubtype);
	
	BEACON_VAR_PRINT(myLANbeacon.VLAN_id);
	
	BEACON_VAR_PRINT(myLANbeacon.VLAN_name_length);
	
	BEACON_VAR_PRINT(myLANbeacon.TLVsubtype);
	
	BEACON_STR_PRINT(myLANbeacon.TLVinformationString[TLV_INFO_VLAN_NAME]);
	
	BEACON_STR_PRINT(myLANbeacon.TLVinformationString[TLV_INFO_FLIESSTEXT]);
	
	BEACON_VAR_PRINT(myLANbeacon.stringLengths[TLV_INFO_FLIESSTEXT]);
	
	printf("Size of LANbeacon: %zu\n\n",sizeof(myLANbeacon));
//	BEACON_VAR_PRINT(myLANbeacon);
}
