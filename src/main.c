#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"

/* 
- to run enter:
clear && make && clear && LANbeacon -n blabla -i 4050 && xxd -b -c 8 binBeacon && echo '\n' && xxd -b -c 8 combinedBeacon

- for generating TLV:
xxd -ps combinedBeacon | fold -w2 | paste -sd',' -
configure lldp custom-tlv add oui cc,4d,55 subtype 44 oui-info xxxxxxxx

- saving TCPdump: tcpdump -s 65535 -w meindump ether proto 0x88cc
*/

int main(int argc, char **argv) {
	
	struct LANbeaconProperties *myLANbeaconProperties = setLANbeaconProperties(&argc, argv);
	
	struct LANbeacon *myLANbeacon = createLANbeacon(myLANbeaconProperties);
	
//	printLANbeacon(*myLANbeacon);
	
	// ###### SPIELWIESE ######
	//	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
	return 0;
}
