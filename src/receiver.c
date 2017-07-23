/** @cond */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <libintl.h>
#include <locale.h>
#include <getopt.h>
/** @endcond */

#include "openssl_sign.h"
#include "cberry_includes_tft.h"
#include "cberry_includes_RAIO8870.h"
#include "rawsocket_LAN_Beacon.h"
#include "receiver.h"
#include "sender.h"
#include "define.h"
#include "main.h"


// copying TLV contents to collected parsed strings:
#define TLV_CUSTOM_COPY(descriptor, TLV_parsed_content, makro_currentTLVcontentSize) \
	snprintf(parsedTLVs [numberParsedTLVs++], PARSED_TLVS_MAX_LENGTH, "%-10s%.*s",  \
		descriptor, (int) makro_currentTLVcontentSize, TLV_parsed_content); \
	break;

// shortcut for transferring TLVs that only contain string
// +6 because header 2 + OUI 3 + Subtype 1, -4 due to 3 OUI + 1 Subtype
#define TLV_STRING_COPY(descriptor) \
	TLV_CUSTOM_COPY(descriptor,  \
		(char*) &my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+6], \
		currentTLVsize-4);
	
int receiver(int argc, char **argv) {
	
	// initialize receiver struct
	struct receiver_information my_receiver_information = {
		.current_lan_beacon_pdu_for_printing = 0,
		.authenticated_mode = 0,
		.number_of_currently_received_frames = 0,
		.scroll_speed = DEFAULT_SCROLLSPEED,
		
		.lanbeacon_keys = {
			.path_To_Verifying_Key = PUBLIC_KEY_STANDARD_PATH,
		},
		
		.my_receiver_interfaces = {
			.maxSockFd = 0,
			.numInterfaces = 0,
			.etherType = LAN_BEACON_ETHER_TYPE,
			.sendOrReceive = REC_SOCKET
		}
	};
	
	my_receiver_information.lanbeacon_keys.sender_or_receiver_mode = RECEIVER_MODE; 
	
	int opt;
	// if listen mode is enabled, get all arguments.
	// if any arguments are contained, which are not used in listen mode, show help
	while((opt=getopt(argc, argv, "lav:y:")) != -1) {
		switch(opt) {

			case 'l':
				break;

			case 'a':
				my_receiver_information.authenticated_mode = 1;
				break;

			case 'y':
				my_receiver_information.scroll_speed = atoi(optarg);
				break;

			case 'v':
				if (strlen(optarg) > KEY_PATHLENGTH_MAX) {
					puts(_("Passed path to verifying key too long. Exiting."));
					return EXIT_FAILURE;
				}
				strncpy(
					my_receiver_information.lanbeacon_keys.path_To_Verifying_Key,
					optarg, KEY_PATHLENGTH_MAX
				);
				my_receiver_information.lanbeacon_keys.path_To_Verifying_Key[KEY_PATHLENGTH_MAX] = 0;
				break;

			case 'h':
				printHelp();

			default:
				printHelp();
		}
	}

	getInterfaces (&my_receiver_information.my_receiver_interfaces, NULL);
	
	while (1) {
		// receive new lanbeacons
		lan_beacon_receiver (&my_receiver_information);
		// print everything, that just was received
		bananaPIprint(&my_receiver_information);
	}
	
	// free memory
	for (int j = 0; j < my_receiver_information.number_of_currently_received_frames; j++) {
		
		for (int i = 0 ; i < PARSED_TLVS_MAX_NUMBER; ++i) {
			free(my_receiver_information.pointers_to_received_frames[j]->parsedBeaconContents[i]);
		}
		free(my_receiver_information.pointers_to_received_frames[j]->parsedBeaconContents);
		free(my_receiver_information.pointers_to_received_frames[j]);
	}

	return EXIT_SUCCESS;
}




char ** evaluatelanbeacon (struct received_lan_beacon_frame *my_received_lan_beacon_frame, struct open_ssl_keys *lanbeacon_keys) {

//	char parsedTLVs [PARSED_TLVS_MAX_NUMBER][PARSED_TLVS_MAX_LENGTH];
	char ** parsedTLVs = malloc(PARSED_TLVS_MAX_NUMBER * sizeof(char*));
	if(!parsedTLVs) puts(_("malloc error of \"parsedTLVs\" in evaluatelanbeacon"));
	for (int i =0 ; i < PARSED_TLVS_MAX_NUMBER; ++i) {
		parsedTLVs[i] = calloc(PARSED_TLVS_MAX_LENGTH, sizeof(char));
		if(!parsedTLVs[i]) puts(_("malloc error of \"parsedTLVs\" in evaluatelanbeacon"));
	}


	// position after headers and mandatory TLVs: 36, postition after Ethernet: 14
	unsigned int currentPayloadByte = 14;
	unsigned int currentTLVsize = 0;

	unsigned int numberParsedTLVs = 0;
	unsigned int currentLabelSize = 0;
	char TLVstringbuffer[500] = "";

	while ( currentPayloadByte < my_received_lan_beacon_frame->payloadSize - 2 ) {

		currentTLVsize = my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+1] + 
			(0b100000000 * (my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte] & 1));

		if (127 == (my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte] >> 1)
		&& (my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+2] == ( 'L' | 0b10000000)
		&&  my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+3] == 'M'
		&&  my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+4] == 'U'  ) ) {

			switch(my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+5]) {
				case SUBTYPE_VLAN_ID:

					TLVstringbuffer[0] = 0;
					unsigned short int VLAN_id;
					memcpy (&VLAN_id, 
						&my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+6], 2);
					VLAN_id = ntohs(VLAN_id);

					sprintf(TLVstringbuffer, "%hu", VLAN_id);
					TLV_CUSTOM_COPY( DESCRIPTOR_VLAN_ID, TLVstringbuffer, strlen(TLVstringbuffer));

				case SUBTYPE_NAME:
					TLV_STRING_COPY(DESCRIPTOR_NAME);

				case SUBTYPE_CUSTOM:
					TLV_STRING_COPY(DESCRIPTOR_CUSTOM);

				case SUBTYPE_IPV4:

					TLVstringbuffer[0] = 0;
					char currentIP4[4] = "";
					char currentIP4string[50] = "";

					for (int i = 6; i < currentTLVsize; i += 5) {
						// get IP address
						memcpy (currentIP4, 
							&my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+i], 4);
						// convert binary representation to string
						inet_ntop(AF_INET, currentIP4, currentIP4string, 20);
						// get IP address, then subNetwork
						sprintf(currentIP4string, "%s/%i, ", currentIP4string, 
							my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+i+4]);
						strcat (TLVstringbuffer, currentIP4string);
					}

					// remove last comma and space
					TLVstringbuffer[strlen(TLVstringbuffer)-2] = 0;

					TLV_CUSTOM_COPY( DESCRIPTOR_IPV4, TLVstringbuffer, strlen(TLVstringbuffer));

				case SUBTYPE_IPV6:

					TLVstringbuffer[0] = 0;
					char currentIP6[16] = "";
					char currentIP6string[100] = "";

					for (int i = 6; i < currentTLVsize; i += 17) {
						// get IP address
						memcpy (currentIP6, 
							&my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+i], 16);	
						
						// convert binary representation to string
						inet_ntop(AF_INET6, currentIP6, currentIP6string, 100);	

						// get IP address, then subNetwork
						sprintf(currentIP6string, "%s/%i, ", currentIP6string, 
							my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+i+16]);		
						strcat (TLVstringbuffer, currentIP6string);
					}

					// remove last comma and space
					TLVstringbuffer[strlen(TLVstringbuffer)-2] = 0;		

					TLV_CUSTOM_COPY( DESCRIPTOR_IPV6, TLVstringbuffer, strlen(TLVstringbuffer));

				case SUBTYPE_EMAIL:
					TLV_STRING_COPY( DESCRIPTOR_EMAIL);

				case SUBTYPE_DHCP:
					TLV_STRING_COPY( DESCRIPTOR_DHCP);

				case SUBTYPE_ROUTER:
					TLV_STRING_COPY( DESCRIPTOR_ROUTER);

				case SUBTYPE_COMBINED_STRING:
					TLV_STRING_COPY( DESCRIPTOR_COMBINED_STRING);

				case SUBTYPE_SIGNATURE:

					TLVstringbuffer[0] = 0;
					time_t timeStamp = time(NULL);

					long zwischenSpeicherChallenge, zwischenSpeicherTimeStamp;
					
					// Verify signature
					// position: end of LAN-Beacon PDU - 2 LAN-Beacon PDU end TLV - 14 Ethernet header
					if (0 != verifylanbeacon(
						&my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[14],
						my_received_lan_beacon_frame->payloadSize - 2 - 14, lanbeacon_keys)) {
							puts(_("problem with signature verification"));
						sprintf(TLVstringbuffer,
							_(" failed. Signature could not be verified."));
						TLV_CUSTOM_COPY( DESCRIPTOR_SIGNATURE, TLVstringbuffer, strlen(TLVstringbuffer));
						continue;
					}

					memcpy (&zwischenSpeicherChallenge,
						&my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+6], 4);
					zwischenSpeicherChallenge = ntohl(zwischenSpeicherChallenge);
					memcpy (&zwischenSpeicherTimeStamp,
						&my_received_lan_beacon_frame->lan_beacon_ReceivedPayload[currentPayloadByte+6+4], 4);
					zwischenSpeicherTimeStamp = ntohl(zwischenSpeicherTimeStamp);

					if ((((unsigned long) ntohl(my_received_lan_beacon_frame->challenge))
					== zwischenSpeicherChallenge) &&
					(timeStamp - zwischenSpeicherTimeStamp < 10)) {
						sprintf(TLVstringbuffer,
							_(" successfull."));
						my_received_lan_beacon_frame->successfullyAuthenticated = 1;
					} else {
						sprintf(TLVstringbuffer,
						_(" failed. Sent challenge: %ld Received Challenge: %ld Timestamp: %ld."),
						(unsigned long) ntohl(my_received_lan_beacon_frame->challenge), zwischenSpeicherChallenge, zwischenSpeicherTimeStamp);
					}
					
					TLV_CUSTOM_COPY( DESCRIPTOR_SIGNATURE, TLVstringbuffer, strlen(TLVstringbuffer));
			}

		}

		currentPayloadByte += currentTLVsize + 2;	// + TLVheader
	}

	printf("\n #####Parsed TLVs: %i#####\n", numberParsedTLVs);
	for (int i = 0; i < numberParsedTLVs; i++)
		printf("%s#\n",parsedTLVs[i]);

	return parsedTLVs;
}



void bananaPIprint (struct receiver_information *my_receiver_information) {

	#ifdef BANANAPI_SWITCH
	TFT_init_board();
	TFT_hard_reset();
	RAIO_init();
	RAIO_clear_screen();
	#endif
	
	char buf[800];
	int c = 0;
	int column;

	int currentPosInTLV, currentPIline, endOfLastPartialString;

	clock_t begin, end;

	int currentLastSpace = 0;

	for (my_receiver_information->current_lan_beacon_pdu_for_printing = 0; 
			my_receiver_information->current_lan_beacon_pdu_for_printing < my_receiver_information->number_of_currently_received_frames; 
			my_receiver_information->current_lan_beacon_pdu_for_printing++) {

		currentPIline = 0;

		printf("\e[1;1H\e[2J"); //clear screen

		#ifdef BANANAPI_SWITCH
		RAIO_clear_screen();
		#endif
		
		for (int currentTLV = 0; currentTLV < PARSED_TLVS_MAX_NUMBER; currentTLV++) {

			for (currentPosInTLV = 1, endOfLastPartialString = 0; 
			my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]->parsedBeaconContents[currentTLV][currentPosInTLV-1] != 0 ; currentPosInTLV++) {

				if (my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]->parsedBeaconContents[currentTLV][currentPosInTLV] == ' ')
					currentLastSpace = currentPosInTLV;

				if (my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]->parsedBeaconContents[currentTLV][currentPosInTLV] == 0
				||	currentPosInTLV - endOfLastPartialString 
				> (endOfLastPartialString == 0 ? 39 : 39 - DESCRIPTOR_WIDTH)) {

					// avoid newline in the middle of word, in case there was a space 
					// character in the current string we will put the new line there
					if (currentLastSpace != 0
					&& (currentPosInTLV - endOfLastPartialString 
					> (endOfLastPartialString == 0 ? 39 : 39 - DESCRIPTOR_WIDTH)))
						currentPosInTLV = currentLastSpace + 1;

					printf(_("Line %i:  \t"), currentPIline);

					if (endOfLastPartialString == 0) {
						snprintf(buf, currentPosInTLV - endOfLastPartialString + 1, 
							"%s", &my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]->parsedBeaconContents[currentTLV][endOfLastPartialString]);
					}
					else {
						snprintf(buf, currentPosInTLV - endOfLastPartialString + 1 + DESCRIPTOR_WIDTH, 
							"%*s%s", DESCRIPTOR_WIDTH, "", 
							&my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]->parsedBeaconContents[currentTLV][endOfLastPartialString]);
					}

					puts(buf);

					#ifdef BANANAPI_SWITCH
					RAIO_SetFontSizeFactor( 0 );
					RAIO_print_text(0, 16*currentPIline, (unsigned char*) buf, 
									COLOR_BLACK, COLOR_WHITE);
					#endif

					endOfLastPartialString = currentPosInTLV;
					currentLastSpace = 0;

					if (currentPIline++ >= 14) {
						sleep (my_receiver_information->scroll_speed);
						currentPIline = 0;

						printf("\e[1;1H\e[2J"); //clear screen

						#ifdef BANANAPI_SWITCH
						RAIO_clear_screen();
						#endif
					}
				}
			}
		}

		//if sleep (5) already has been executed, don't execute again
		if (currentPIline != 0)	sleep (my_receiver_information->scroll_speed);
		
		// check, how many times frame has been printed yet
		if (--my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]->times_left_to_display == 0) {
			// remove frame
			free(my_receiver_information->pointers_to_received_frames[my_receiver_information->current_lan_beacon_pdu_for_printing]);
			
			for (int x = my_receiver_information->current_lan_beacon_pdu_for_printing; 
					x < my_receiver_information->number_of_currently_received_frames; 
					x++) {
				my_receiver_information->pointers_to_received_frames[x] = my_receiver_information->pointers_to_received_frames[x+1];
			}
			
			my_receiver_information->current_lan_beacon_pdu_for_printing--;
			my_receiver_information->number_of_currently_received_frames--;
		}
	}

	return;
}

