#include <libintl.h>
#include <locale.h>
#include <stdlib.h>     // getenv
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "sender.h"
#include "rawsocket_LLDP.h"
#include "receiver.h"
#include "define.h"

/* 
- to run enter:
clear && make && clear && ./LANbeacon -4 "Gobi: 192.168.178.133/24 Arktis: 111.222.111.222/16 Kalahari: 222.111.222.111/17" -6 "LRZ: 2001:618:11:1:1::1/127 MNM: 2001:cdba:0:0:0:0:3257:9652/33" -e "dominik.bitzer@mailbox.org" -d "DHCP info" -r "MAC: 00:04:4b:01:70:aa" -i 937 -n "MNM-VLAN Team IPsec" -c 'Das ist ein Beispiel fuer einen benutzerdefinierten String. Es kann beliebiger Text mitgegeben werden' && echo && xxd  -c 16 testNewTransfer

- saving TCPdump: tcpdump -s 65535 -w meindump ether proto 0x88cc
*/

int main(int argc, char **argv) {
	setlocale (LC_ALL, "");
	char currentL10nFolder[200];
	sprintf(currentL10nFolder, "%s%s", getenv("PWD"), "/l10n");
	bindtextdomain ("LANbeacon", currentL10nFolder); // "/usr/share/locale/");
	textdomain ("LANbeacon");
	
	//## receiving LANbeacon ##//
	if (argc > 1 && strcmp("-r", argv[1]) == 0) {
		unsigned char LLDPreceivedPayload[LLDP_BUF_SIZ];
		ssize_t payloadSize;
		recLLDPrawSock(LLDPreceivedPayload, &payloadSize);
		
//debug//		printf ("LLDPDU_len: %lu\n",payloadSize); FILE *combined = fopen("received_raw_beacon","w");	fwrite(LLDPreceivedPayload, payloadSize, 1, combined);
		
		char ** parsedBeaconContents = evaluateLANbeacon(LLDPreceivedPayload, payloadSize);
		
		bananaPIprint(parsedBeaconContents);
		
		return 1;
	}
	
	
	//## creating and sending LANbeacon
	int LLDPDU_len;
	char *LANbeaconCustomTLVs = mergedLANbeaconCreator(&argc, argv, &LLDPDU_len);
	
	sendLLDPrawSock (LLDPDU_len, LANbeaconCustomTLVs);
	
	// ###### SPIELWIESE ######
	//	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
//debug//
	printf ("LLDPDU_len: %i\n",LLDPDU_len);	FILE *combined = fopen("testNewTransfer","w");	fwrite(LANbeaconCustomTLVs, LLDPDU_len, 1, combined);
	
	return 0;
}
