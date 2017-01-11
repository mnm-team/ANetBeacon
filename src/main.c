
#include <stdio.h>
#include <unistd.h>
#include "beacon.h"


int main(int argc, char **argv) {

	printf("%zu\n", sizeof(short unsigned int));
	
	printf("%x\n",99);
	
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
