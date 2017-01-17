#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"

/* to run enter:
clear && make && clear && LANbeacon -n blabla -i 4050 && xxd -b -c 8 binBeacon && echo '\n' && xxd -b -c 8 combinedBeacon
*/

int main(int argc, char **argv) {
	
	struct LANbeaconProperties *myLANbeaconProperties = setLANbeaconProperties(&argc, argv);
	
	struct LANbeacon *myLANbeacon = createLANbeacon(myLANbeaconProperties);
	combineBeacon(myLANbeacon);
	
	
	printLANbeacon(*myLANbeacon);
	
	// ###### SPIELWIESE ######
	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
	return 0;
}
