#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"

int main(int argc, char **argv) {
	
	struct LANbeaconProperties *myLANbeaconProperties = setLANbeaconProperties(&argc, argv);
	
	struct LANbeacon *myLANbeacon = createLANbeacon(myLANbeaconProperties); 
	printLANbeacon(*myLANbeacon);
	combineBeacon(*myLANbeacon);
	
	
	
	// ###### SPIELWIESE ######
	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
	return 0;
}
