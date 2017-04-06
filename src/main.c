#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"
#include "mergedBeacon.h"
#include "sendLLDPrawSock.h"

/* 
- to run enter:
clear && make && clear && LANbeacon -n blabla -i 4050 && xxd -b -c 8 binBeacon && echo '\n' && xxd -b -c 8 combinedBeacon
clear && make && clear && ./LANbeacon -4 "192.168.178.133/24" -6 "2001:618:11:1:1::1/127" -e "dominik.bitzer@mailbox.org" -d "DHCP info" -r "MAC: 00:04:4b:01:70:aa" -i 123 -o LMU -n MNM-VLAN -c 'Das ist ein Beispiel fuer einen benutzerdefinierten String.' && echo && xxd -c 20 combinedBeacon

- for generating TLV:
xxd -ps combinedBeacon | fold -w2 | paste -sd',' -
configure lldp custom-tlv add oui cc,4d,55 subtype 217 oui-info xxxxxxxx

- unconfigure: lldpcli unconfigure lldp custom-tlv

- all in all: lldpcli unconfigure lldp custom-tlv && lldpcli configure lldp custom-tlv add oui cc,4d,55 subtype 44 oui-info `xxd -ps combinedBeacon | fold -w2 | paste -sd',' -` && lldpcli update

- saving TCPdump: tcpdump -s 65535 -w meindump ether proto 0x88cc
*/

int main(int argc, char **argv) {
	
	int LLDPDU_len;
	char *LANbeaconCustomTLVs = mergedLANbeaconCreator(&argc, argv, &LLDPDU_len);
	
	while (1) {
		sendLLDPrawSock (LLDPDU_len, LANbeaconCustomTLVs);
		sleep(5);
	}
	
	// ###### SPIELWIESE ######
	//	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
	printf ("LLDPDU_len: %i\n",LLDPDU_len);
	FILE *combined = fopen("testNewTransfer","w"); fwrite(LANbeaconCustomTLVs, LLDPDU_len, 1, combined);
	
	return 0;
}
