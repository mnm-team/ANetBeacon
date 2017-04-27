#include <libintl.h>
#include <locale.h>
#include <stdlib.h>     // getenv
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "openssl_sign.h"
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
	
	//## gettext setup
	setlocale (LC_ALL, "");
	char currentL10nFolder[200];
	sprintf(currentL10nFolder, "%s%s", getenv("PWD"), "/l10n");
	bindtextdomain ("LANbeacon", currentL10nFolder); // "/usr/share/locale/");
	textdomain ("LANbeacon");
	
	
	char listenerBool = 0;
	
	struct open_ssl_keys lanbeacon_keys = {.path_To_Verifying_Key = "pubkey.pem", .path_To_Signing_Key = "privkey.pem" };
	
//printf("%s\n%s\n", lanbeacon_keys.path_To_Verifying_Key, lanbeacon_keys.path_To_Signing_Key);
	
	char path_To_Verifying_Key[KEY_PATHLENGTH_MAX] = "pubkey.pem";
	char path_To_Signing_Key[KEY_PATHLENGTH_MAX] = "privkey.pem";

	// looking for listener flag and reading verifying key path
	if (argc > 1 && ((strcmp("-l", argv[1]) == 0) || (argc > 2 && strcmp("-l", argv[2]) == 0))) {
		
		if (0 == strcmp("-v", argv[1])) {
			if (strlen(argv[1]) > KEY_PATHLENGTH_MAX) {
				puts(_("Passed path to verifying key too long. Exiting."));
				return EXIT_FAILURE; 
			}
			strncpy(lanbeacon_keys.path_To_Verifying_Key, argv[1], KEY_PATHLENGTH_MAX);
			lanbeacon_keys.path_To_Verifying_Key[strlen(argv[1])-1] = 0;
		}
		
		if ((argc > 2) && (0 == strcmp("-v", argv[2]))) {
			if (strlen(argv[2]) > KEY_PATHLENGTH_MAX) {
				puts(_("Passed path to verifying key too long. Exiting."));
				return EXIT_FAILURE; 
			}
			strncpy(lanbeacon_keys.path_To_Verifying_Key, argv[2], KEY_PATHLENGTH_MAX);
			lanbeacon_keys.path_To_Verifying_Key[strlen(argv[2])-1] = 0;
		}

		//## receiving LANbeacon
		unsigned char lldpReceivedPayload[LLDP_BUF_SIZ];
		ssize_t payloadSize;
	
		recLLDPrawSock(lldpReceivedPayload, &payloadSize, &lanbeacon_keys);
	
//debug//		printf ("LLDPDU_len: %lu\n",payloadSize); FILE *combined = fopen("received_raw_beacon","w");	fwrite(lldpReceivedPayload, payloadSize, 1, combined);
		
		char ** parsedBeaconContents = evaluateLANbeacon(lldpReceivedPayload, payloadSize);
	
		bananaPIprint(parsedBeaconContents, &lanbeacon_keys);
	
	}
	

	
	else {
		//## creating and sending LANbeacon
		int lldpdu_len;
		char *lanBeaconCustomTLVs = mergedLANbeaconCreator(&argc, argv, &lldpdu_len, &lanbeacon_keys);
	
		sendLLDPrawSock (lldpdu_len, lanBeaconCustomTLVs);
	}
	
	
	// ###### SPIELWIESE ######
	//	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
//debug//	printf ("lldpdu_len: %i\n",lldpdu_len);	FILE *combined = fopen("testNewTransfer","w");	fwrite(lanBeaconCustomTLVs, lldpdu_len, 1, combined);
	
	return EXIT_SUCCESS;
}
