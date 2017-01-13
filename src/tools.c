
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


void binPrint(size_t const size, void const * const ptr)	// JUST FOR TESTING!!
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;
	int i, j;
	
	unsigned int *myInt = (unsigned int*) ptr;
	

	printf("Binary:\t");
	for (i=1-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
		printf(" ");
		
		if ((i>0)&&((size-i)%4 == 0)) {printf("\n\t");}		//Newline each 4 blocks
	}
	
}


