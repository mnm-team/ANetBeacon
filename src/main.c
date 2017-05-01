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
	
	struct open_ssl_keys lanbeacon_keys = {.path_To_Verifying_Key = "pubkey.pem", .path_To_Signing_Key = "privkey.pem" };
//printf("%s\n%s\n", lanbeacon_keys.path_To_Verifying_Key, lanbeacon_keys.path_To_Signing_Key);
	
	int opt;
	for (int current_arg = 1; current_arg < argc; current_arg++) {
		if (strcmp("-l", argv[current_arg]) == 0) {
			while((opt=getopt(argc, argv, "lv:")) != -1) {
				switch(opt) {
			
					case 'l':
						break;
			
					case 'v':	//## TLV VLAN ID ##//
						if (strlen(argv[current_arg]) > KEY_PATHLENGTH_MAX) {
							puts(_("Passed path to verifying key too long. Exiting."));
							return EXIT_FAILURE; 
						}
						strncpy(lanbeacon_keys.path_To_Verifying_Key, argv[current_arg], KEY_PATHLENGTH_MAX);
						lanbeacon_keys.path_To_Verifying_Key[strlen(argv[current_arg])-1] = 0;
						break;
			
					case 'h':
						printHelp();
			
					default:
						printHelp();
				}
			}
		
		//## receiving LANbeacon
		struct received_lldp_packet *my_received_lldp_packet = recLLDPrawSock(&lanbeacon_keys);
		char ** parsedBeaconContents = evaluateLANbeacon(my_received_lldp_packet);
		bananaPIprint(parsedBeaconContents, &lanbeacon_keys);
		return EXIT_SUCCESS;
		}
	}
		
	//## creating and sending LANbeacon
	int lldpdu_len;
	char *lanBeaconCustomTLVs = mergedLANbeaconCreator(&argc, argv, &lldpdu_len, &lanbeacon_keys);

	sendLLDPrawSock (lldpdu_len, lanBeaconCustomTLVs, &lanbeacon_keys);
	
	
	//	###### SPIELWIESE ######
	//	printf("\n\n##########\nSPIELWIESE\n##########\n\n\n");
	
//debug//	printf ("lldpdu_len: %i\n",lldpdu_len);	FILE *combined = fopen("testNewTransfer","w");	fwrite(lanBeaconCustomTLVs, lldpdu_len, 1, combined);
	
	return EXIT_SUCCESS;
}
