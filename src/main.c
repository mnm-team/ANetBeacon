/** @cond */
#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
/** @endcond */

#include "openssl_sign.h"
#include "sender.h"
#include "rawsocket_LAN_Beacon.h"
#include "receiver.h"
#include "define.h"
#include "main.h"

/*
- to run Server enter:
./lanbeacon -4 "Gobi: 192.168.178.133/24 Arktis: 111.222.111.222/16 Kalahari: 222.111.222.111/17" -6 "LRZ: 2001:618:11:1:1::1/127 MNM: 2001:cdba:0:0:0:0:3257:9652/33" -e "dominik.bitzer@mailbox.org" -d "DHCP info" -r "MAC: 00:04:4b:01:70:aa" -i 937 -n "MNM-VLAN Team IPsec" -c 'Das ist ein Beispiel fuer einen benutzerdefinierten String. Es kann beliebiger Text mitgegeben werden' -p "sample_password"

- to run Client in authenticated mode enter: ./lanbeacon -l -a

- saving TCPdump: tcpdump -s 65535 -w dump_file ether proto 0x88B5
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


	// check if any argument is "l", in which case the "listen" mode will be used
	for (int current_arg = 1; current_arg < argc; current_arg++) {
		if (strcmp("-l", argv[current_arg]) == 0) {
			// lanbeacon listener mode
			receiver(argc, argv);
			return EXIT_SUCCESS;
		}
		else {
			//lanbeacon sender mode
			
			sender(argc, argv);
			return EXIT_SUCCESS;
		}
		
	}


}

void printHelp() {
	printf( "%s%s", _("Usage: "),
		"\t./lanbeacon [-i VLAN_ID] [-n VLAN_NAME] [-4 IPv4_SUBNETWORK] [-6 IPv6_SUBNETWORK]"
		"[-e EMAIL_CONTACTPERSON] [-d DHCP_TYPES] [-r ROUTER_INFORMATION] [-c CUSTOM_STRING]"
		"[-f SENDING_INTERFACE] [-g] -p PRIVATE_KEY_PASSWORD [-s PATH_TO_PRIVATE_KEY]"
		"[-v PATH_TO_PUBLIC_KEY] [-z SEND_FREQUENCY]");
	printf("\t./lanbeacon -l [-a] [-v PATH_TO_PUBLIC_KEY] [-y SCROLL_SPEED]\n");
	printf("\t./client -h\n");
	exit(EXIT_FAILURE);
}

