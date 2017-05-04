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

#include "openssl_sign.h"
#include "cberry_includes_tft.h"
#include "cberry_includes_RAIO8870.h"
#include "rawsocket_LLDP.h"
#include "receiver.h"
#include "sender.h"
#include "define.h"

/*
// copying TLV contents to collected parsed strings:
#define TLV_CUSTOM_COPY(descriptor, TLV_parsed_content, makro_currentTLVcontentSize) \
	sprintf(parsedTLVs [numberParsedTLVs], "%-10s", descriptor); \
	currentLabelSize = strlen(parsedTLVs [numberParsedTLVs]); \
	strncat(parsedTLVs [numberParsedTLVs], TLV_parsed_content, makro_currentTLVcontentSize); \
	parsedTLVs [numberParsedTLVs++][makro_currentTLVcontentSize+currentLabelSize] = 0;	\
	break;	
*/

// copying TLV contents to collected parsed strings:
#define TLV_CUSTOM_COPY(descriptor, TLV_parsed_content, makro_currentTLVcontentSize) \
	snprintf(parsedTLVs [numberParsedTLVs++], PARSED_TLVS_MAX_LENGTH, "%-10s%.*s", descriptor, (int) makro_currentTLVcontentSize, TLV_parsed_content); \
	break;	

// shortcut for transferring TLVs that only contain string
#define TLV_STRING_COPY(descriptor) \
	TLV_CUSTOM_COPY(descriptor, (char*) &my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+6], currentTLVsize-4);	/* +6 because header 2 + OUI 3 + Subtype 1 */	 /* -4 due to 3 OUI + 1 Subtype */
	 

char ** evaluatelanbeacon (struct received_lldp_packet *my_received_lldp_packet) {
	
//	char parsedTLVs [PARSED_TLVS_MAX_NUMBER][PARSED_TLVS_MAX_LENGTH];
	char ** parsedTLVs = malloc(PARSED_TLVS_MAX_NUMBER * sizeof(char*));
	if(!parsedTLVs) puts(_("malloc error of \"parsedTLVs\" in evaluatelanbeacon"));
	for (int i =0 ; i < PARSED_TLVS_MAX_NUMBER; ++i) {
		parsedTLVs[i] = calloc(PARSED_TLVS_MAX_LENGTH, sizeof(char));
		if(!parsedTLVs[i]) puts(_("malloc error of \"parsedTLVs\" in evaluatelanbeacon"));
	}


	unsigned int currentPayloadByte = 14;	// position after headers and mandatory TLVs: 36, postition after Ethernet: 14
	unsigned int currentTLVsize = 0;
	
	unsigned int numberParsedTLVs = 0;
	unsigned int currentLabelSize = 0;
	char TLVstringbuffer[500] = "";
	
	while ( currentPayloadByte < my_received_lldp_packet->payloadSize - 2 ) {

		currentTLVsize = my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+1] + (0b100000000 * (my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte] & 1));
		
//		printf (" Pos: %i  	Type: %i   	Size: %i#\n", currentPayloadByte, (my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte] >> 1), currentTLVsize);
		
		if (127 == (my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte] >> 1)
		&& (my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+2] == ( 'L' | 0b10000000)
		&&  my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+3] == 'M'
		&&  my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+4] == 'U'  ) ) {
		
			switch(my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+5]) {		// Subtype
				case SUBTYPE_VLAN_ID:
					
					TLVstringbuffer[0] = 0;
					unsigned short int VLAN_id;
					memcpy (&VLAN_id, &my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+6], 2);
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
						memcpy (currentIP4, &my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+i], 4);	// get IP address
						inet_ntop(AF_INET, currentIP4, currentIP4string, 20);	// convert binary representation to string
						sprintf(currentIP4string, "%s/%i, ", currentIP4string, my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+i+4]);		// get IP address, then subNetwork
						strcat (TLVstringbuffer, currentIP4string);
					}
					
					TLVstringbuffer[strlen(TLVstringbuffer)-2] = 0;		// remove last comma and space
					
					TLV_CUSTOM_COPY( DESCRIPTOR_IPV4, TLVstringbuffer, strlen(TLVstringbuffer));
					
				case SUBTYPE_IPV6:
					
					TLVstringbuffer[0] = 0;
					char currentIP6[16] = "";
					char currentIP6string[100] = "";
					
					for (int i = 6; i < currentTLVsize; i += 17) {
						memcpy (currentIP6, &my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+i], 16);	// get IP address
						inet_ntop(AF_INET6, currentIP6, currentIP6string, 100);	// convert binary representation to string
						
						sprintf(currentIP6string, "%s/%i, ", currentIP6string, my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+i+16]);		// get IP address, then subNetwork
						strcat (TLVstringbuffer, currentIP6string);
					}
					
					TLVstringbuffer[strlen(TLVstringbuffer)-2] = 0;		// remove last comma and space
					
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
					
					long zwischenSpeicherChallenge, zwischenSpeicherTimeStamp;
					
					memcpy (&zwischenSpeicherChallenge, &my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+6], 4);
					zwischenSpeicherChallenge = ntohl(zwischenSpeicherChallenge);
					memcpy (&zwischenSpeicherTimeStamp, &my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+6+4], 4); 
					zwischenSpeicherTimeStamp = ntohl(zwischenSpeicherTimeStamp);
					
					sprintf(TLVstringbuffer, _("AUTHENTICATION SUCCESSFULL! Challenge: %ld Timestamp: %ld"), zwischenSpeicherChallenge, zwischenSpeicherTimeStamp);
					
					printf("AuthTest %s\n", TLVstringbuffer);
					
					TLV_CUSTOM_COPY( DESCRIPTOR_SIGNATURE, TLVstringbuffer, strlen(TLVstringbuffer));
					
			}
			
			
			
//			printf ("*Pos: %i  	Type: %i   	Size: %i  	Subtype: %i 	Content: %s#\n", currentPayloadByte, (my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte] >> 1), currentTLVsize, my_received_lldp_packet->lldpReceivedPayload[currentPayloadByte+5], parsedTLVs [numberParsedTLVs-1]);	//	, &currentTLVcontents[4]
			
		}
		
//		memset(currentTLVcontents, 0, currentTLVsize+1);
		currentPayloadByte += currentTLVsize + 2;	// + TLVheader
	}
	
	printf("\n #####Parsed TLVs: %i#####\n", numberParsedTLVs);
	for (int i = 0; i < numberParsedTLVs; i++)
		printf("%s#\n",parsedTLVs[i]);
	
	return parsedTLVs;
}



void bananaPIprint (char **parsedBeaconContents, struct open_ssl_keys *lanbeacon_keys) {
	
	#ifdef BANANAPI_SWITCH
	TFT_init_board();
	TFT_hard_reset();
	RAIO_init();
	RAIO_clear_screen();
	#endif
//printf("Ausgabe: %s\n",lanbeacon_keys->path_To_Verifying_Key);	
puts("\n\n\n####PIdisplay: ####");	
	char buf[800];
	int c = 0;
	int column;
	
	int currentPosInTLV, currentPIline, endOfLastPartialString;
	
	clock_t begin, end;
	
	int currentLastSpace = 0;
	
	while (1) {
		
		currentPIline = 0;
		
		printf("\e[1;1H\e[2J"); //clear screen
		
		#ifdef BANANAPI_SWITCH
		RAIO_clear_screen();
		#endif
		
		for (int currentTLV = 0; currentTLV < PARSED_TLVS_MAX_NUMBER; currentTLV++) {
		
			for (currentPosInTLV = 1, endOfLastPartialString = 0; parsedBeaconContents[currentTLV][currentPosInTLV-1] != 0 ; currentPosInTLV++) {
			
				if (parsedBeaconContents[currentTLV][currentPosInTLV] == ' ') currentLastSpace = currentPosInTLV; 
			
				if (parsedBeaconContents[currentTLV][currentPosInTLV] == 0
				||	currentPosInTLV - endOfLastPartialString > (endOfLastPartialString == 0 ? 39 : 39 - DESCRIPTOR_WIDTH)) {
				
					// avoid newline in the middle of word, in case there was a space character in the current string we will put the new line there
					if (currentLastSpace != 0 
					&& (currentPosInTLV - endOfLastPartialString > (endOfLastPartialString == 0 ? 39 : 39 - DESCRIPTOR_WIDTH))) 
						currentPosInTLV = currentLastSpace + 1;
				
					printf(_("Line %i:  \t"), currentPIline);
				
					if (endOfLastPartialString == 0) {
						snprintf(buf, currentPosInTLV - endOfLastPartialString + 1, "%s", &parsedBeaconContents[currentTLV][endOfLastPartialString]);
					}
					else {
						snprintf(buf, currentPosInTLV - endOfLastPartialString + 1 + DESCRIPTOR_WIDTH, "%*s%s", DESCRIPTOR_WIDTH, "", &parsedBeaconContents[currentTLV][endOfLastPartialString]);
					}
				
					puts(buf);
				
					#ifdef BANANAPI_SWITCH
					RAIO_SetFontSizeFactor( 0 );
					RAIO_print_text( 0, 16*currentPIline, (unsigned char*) buf, COLOR_BLACK, COLOR_WHITE );
					#endif
				
					endOfLastPartialString = currentPosInTLV;
					currentLastSpace = 0;
				
					if (currentPIline++ >= 14) {
						sleep (5);
						currentPIline = 0;
						
						printf("\e[1;1H\e[2J"); //clear screen
					
						#ifdef BANANAPI_SWITCH
						RAIO_clear_screen();
						#endif
					} 
				}
			}
		}
		
		// TODO umstellen (loop vllt in main?!)
//		struct received_lldp_packet *my_received_lldp_packet = recLLDPrawSock(lanbeacon_keys);
		
//		parsedBeaconContents = evaluatelanbeacon(my_received_lldp_packet);

//		begin = clock();
		
/*		end = clock();
		double time_spent = ((double) (end - begin)) / CLOCKS_PER_SEC;
		
		printf ("Time spent: %f\n", time_spent); 
*/		
		if (currentPIline != 0)	sleep (5);	//if sleep (5) already has been executed, don't execute again
	}
	
	return;
}

