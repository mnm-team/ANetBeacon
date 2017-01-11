
#include <stdio.h>
#include <unistd.h>
#include "tools.h"

void printVarInFormats(size_t const size, void const * const ptr)
{
	unsigned char *b = (unsigned char*) ptr;
	unsigned char byte;
	int i, j;
	
	//Start printing values
	printf("HEX: %x\n", *b);
	printf("DEC: %i\n", *b);

	for (i=size-1;i>=0;i--)
	{
		for (j=7;j>=0;j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
		
		printf(" ");
	}
	puts("");
}

