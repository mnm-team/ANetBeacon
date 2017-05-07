#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
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
clear && make && clear && ./lanbeacon -4 "Gobi: 192.168.178.133/24 Arktis: 111.222.111.222/16 Kalahari: 222.111.222.111/17" -6 "LRZ: 2001:618:11:1:1::1/127 MNM: 2001:cdba:0:0:0:0:3257:9652/33" -e "dominik.bitzer@mailbox.org" -d "DHCP info" -r "MAC: 00:04:4b:01:70:aa" -i 937 -n "MNM-VLAN Team IPsec" -c 'Das ist ein Beispiel fuer einen benutzerdefinierten String. Es kann beliebiger Text mitgegeben werden' && echo && xxd  -c 16 testNewTransfer

- saving TCPdump: tcpdump -s 65535 -w meindump ether proto 0x88cc
*/

int main(int argc, char **argv) {

	// gettext setup
	setlocale (LC_ALL, "");
	char currentL10nFolder[200];
	sprintf(currentL10nFolder, "%s%s", getenv("PWD"), "/l10n");
	bindtextdomain ("lanbeacon", currentL10nFolder); // "/usr/share/locale/"); // TODO
	textdomain ("lanbeacon");

	struct open_ssl_keys lanbeacon_keys = {
		.path_To_Verifying_Key = PUBLIC_KEY_STANDARD_PATH,
		.path_To_Signing_Key = PRIVATE_KEY_STANDARD_PATH,
		.pcszPassphrase = "as" // TODO set default password as empty
	};

	// check if any argument is "listen" mode
	int opt;
	for (int current_arg = 1; current_arg < argc; current_arg++) {
		if (strcmp("-l", argv[current_arg]) == 0) {
			// if listen mode is enabled, get all arguments.
			// if any arguments not used in listen mode are contained, show help
			while((opt=getopt(argc, argv, "lv:p:")) != -1) {
				switch(opt) {

					case 'l':
						break;

					case 'v':
						if (strlen(argv[current_arg]) > KEY_PATHLENGTH_MAX) {
							puts(_("Passed path to verifying key too long. Exiting."));
							return EXIT_FAILURE;
						}
						strncpy(
							lanbeacon_keys.path_To_Verifying_Key,
							argv[current_arg], KEY_PATHLENGTH_MAX
						);
						lanbeacon_keys.path_To_Verifying_Key[strlen(optarg)] = 0;
						break;

					case 'p':
						if (strlen(optarg) > 256) {
							puts(_("Length of passed password too long. Exiting"));
							return EXIT_FAILURE;
						}
						strncpy(lanbeacon_keys.pcszPassphrase, optarg, 256);
						lanbeacon_keys.pcszPassphrase[strlen(optarg)] = 0;
						break;

					case 'h':
						printHelp();

					default:
						printHelp();
				}
			}

		// receive lanbeacon
		struct received_lldp_packet *my_received_lldp_packet
			= recLLDPrawSock(&lanbeacon_keys);
		char ** parsedBeaconContents = evaluatelanbeacon(my_received_lldp_packet);
		bananaPIprint(parsedBeaconContents, &lanbeacon_keys);

		// free memory
		for (int i = 0 ; i < PARSED_TLVS_MAX_NUMBER; ++i) {
			free(parsedBeaconContents[i]);
		}
		free(parsedBeaconContents);
		free(my_received_lldp_packet);

		return EXIT_SUCCESS;
		}
	}

	//## creating and sending lanbeacon
	int lldpdu_len;
	char *lanBeaconCustomTLVs
		= mergedlanbeaconCreator(&argc, argv, &lldpdu_len, &lanbeacon_keys);

	sendLLDPrawSock (lldpdu_len, lanBeaconCustomTLVs, &lanbeacon_keys);

	return EXIT_SUCCESS;
}
