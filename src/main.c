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
- 0x88B6
*/

int main(int argc, char **argv) {

	// gettext setup
	setlocale (LC_ALL, "");
	char currentL10nFolder[200];
	
	//*  <<- Add/remove one '/' here to toggle active code block // TODO
	sprintf(currentL10nFolder, "%s%s", getenv("PWD"), "/l10n");
	/*/
	sprintf(currentL10nFolder, "%s", "/usr/share/locale/");
	//*/
	
	bindtextdomain ("lanbeacon", currentL10nFolder);
	textdomain ("lanbeacon");
	
	

	// lanbeacon listener mode
	// check if any argument is "l", in which case the "listen" mode will be used
	int opt;
	for (int current_arg = 1; current_arg < argc; current_arg++) {
		if (strcmp("-l", argv[current_arg]) == 0) {
			
			struct receiver_information my_receiver_information = {
				.currentLLDPDU_for_printing = 0,
				.authenticated = 0,
				.number_of_currently_received_packets = 0,
				.scroll_speed = DEFAULT_SCROLLSPEED,
				.lanbeacon_keys = {
					.path_To_Verifying_Key = PUBLIC_KEY_STANDARD_PATH,
//					.path_To_Signing_Key = PRIVATE_KEY_STANDARD_PATH,
//					.pcszPassphrase = "TODO" // TODO
				}
			};
			
			my_receiver_information.lanbeacon_keys.sender_or_receiver_mode = RECEIVER_MODE; 
			
			// if listen mode is enabled, get all arguments.
			// if any arguments are contained, which are not used in listen mode, show help
			while((opt=getopt(argc, argv, "lav:y:")) != -1) {
				switch(opt) {

					case 'l':
						break;

					case 'a':
						my_receiver_information.authenticated = 1;
						break;

					case 'y':
						my_receiver_information.scroll_speed = atoi(optarg);
						break;

					case 'v':
						if (strlen(argv[current_arg]) > KEY_PATHLENGTH_MAX) {
							puts(_("Passed path to verifying key too long. Exiting."));
							return EXIT_FAILURE;
						}
						strncpy(
							my_receiver_information.lanbeacon_keys.path_To_Verifying_Key,
							argv[current_arg], KEY_PATHLENGTH_MAX
						);
						my_receiver_information.lanbeacon_keys.path_To_Verifying_Key[KEY_PATHLENGTH_MAX] = 0;
						break;

					case 'h':
						printHelp();

					default:
						printHelp();
				}
			}
		
			// receive lanbeacon
			my_receiver_information.pointers_to_received_packets[0] = recLLDPrawSock(&my_receiver_information);
			my_receiver_information.pointers_to_received_packets[0]->parsedBeaconContents 
				= evaluatelanbeacon(my_receiver_information.pointers_to_received_packets[0]);
			bananaPIprint(&my_receiver_information);

			// free memory
			for (int i = 0 ; i < PARSED_TLVS_MAX_NUMBER; ++i) {
				free(my_receiver_information.pointers_to_received_packets[0]->parsedBeaconContents[i]);
			}
			free(my_receiver_information.pointers_to_received_packets[0]->parsedBeaconContents);
			free(my_receiver_information.pointers_to_received_packets[0]);

			return EXIT_SUCCESS;
		}
	}
	
	
	//lanbeacon sender mode
	//## creating and sending lanbeacon
	struct sender_information my_sender_information = {
		.interface_to_send_on = NULL,
		.lanbeacon_keys = {
			.sender_or_receiver_mode = SENDER_MODE,
			.path_To_Verifying_Key = PUBLIC_KEY_STANDARD_PATH,
			.path_To_Signing_Key = PRIVATE_KEY_STANDARD_PATH,
			.pcszPassphrase = "TODO" // TODO
		}
	};
	my_sender_information.lanBeacon_PDU = mergedlanbeaconCreator(&argc, argv, &my_sender_information);

	sendLLDPrawSock (&my_sender_information);

	if (my_sender_information.interface_to_send_on) free(my_sender_information.interface_to_send_on);
	
	return EXIT_SUCCESS;
}

