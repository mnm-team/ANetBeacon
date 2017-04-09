#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include "beacon.h"
#include "tools.h"
#include "config.h"
#include "mergedBeacon.h"
#include "LLDPrawSock.h"
#include "define.h"


// copying TLV contents to collected parsed strings:
#define TLV_CUSTOM_COPY(descriptor, TLV_parsed_content, makro_currentTLVcontentSize) \
	sprintf(parsedTLVs [numberParsedTLVs], "%-13s", descriptor); \
	currentLabelSize = strlen(parsedTLVs [numberParsedTLVs]); \
	strncat(parsedTLVs [numberParsedTLVs], TLV_parsed_content, makro_currentTLVcontentSize); \
	parsedTLVs [numberParsedTLVs++][makro_currentTLVcontentSize+currentLabelSize] = 0;	/* -4 because 3 OUI and 1 subtype */   \
	break;	

// shortcut for transferring TLVs that only contain string
#define TLV_STRING_COPY(descriptor) \
	TLV_CUSTOM_COPY(descriptor, (char*) &LLDPreceivedPayload[currentPayloadByte+6], currentTLVsize-4);	/* +6 because header 2 + OUI 3 + Subtype 1 */	 /* -4 due to 3 OUI + 1 Subtype */
	 

char ** evaluateLANbeacon (unsigned char *LLDPreceivedPayload, ssize_t payloadSize) {
	
//	char parsedTLVs [PARSED_TLVS_MAX_NUMBER][PARSED_TLVS_MAX_LENGTH];
	char ** parsedTLVs = malloc(PARSED_TLVS_MAX_NUMBER * sizeof(char*));
	for (int i =0 ; i < PARSED_TLVS_MAX_NUMBER; ++i)
		parsedTLVs[i] = malloc(PARSED_TLVS_MAX_LENGTH * sizeof(char));


	unsigned int currentPayloadByte = 14;	// position after headers and mandatory TLVs: 36, postition after Ethernet: 14
	unsigned int currentTLVsize = 0;
	
	unsigned int numberParsedTLVs = 0;
	unsigned int currentLabelSize = 0;
	char TLVstringbuffer[500] = "";
	
	while ( currentPayloadByte < payloadSize - 2 ) {

		currentTLVsize = LLDPreceivedPayload[currentPayloadByte+1] + (0b100000000 * (LLDPreceivedPayload[currentPayloadByte] & 1));
		
//		printf (" Pos: %i  	Type: %i   	Size: %i#\n", currentPayloadByte, (LLDPreceivedPayload[currentPayloadByte] >> 1), currentTLVsize);
		
		if (127 == (LLDPreceivedPayload[currentPayloadByte] >> 1)
		&& (LLDPreceivedPayload[currentPayloadByte+2] == ( 'L' | 0b10000000)
		&&  LLDPreceivedPayload[currentPayloadByte+3] == 'M'
		&&  LLDPreceivedPayload[currentPayloadByte+4] == 'U'  ) ) {
		
			switch(LLDPreceivedPayload[currentPayloadByte+5]) {		// Subtype
				case SUBTYPE_VLAN_ID:
					
					TLVstringbuffer[0] = 0;
					unsigned short int VLAN_id;
					memcpy (&VLAN_id, &LLDPreceivedPayload[currentPayloadByte+6], 2);
					VLAN_id = ntohs(VLAN_id);
					
					sprintf(TLVstringbuffer, "%hu", VLAN_id);
					TLV_CUSTOM_COPY( "VLAN-ID: ", TLVstringbuffer, strlen(TLVstringbuffer));
					
				case SUBTYPE_NAME:
					TLV_STRING_COPY("VLAN-Name: ");
					
				case SUBTYPE_CUSTOM:
					TLV_STRING_COPY("Freitext: ");
					
				case SUBTYPE_IPV4:
					
					TLVstringbuffer[0] = 0;
					char currentIP4[4] = "";
					char currentIP4string[50] = "";
					
					for (int i = 6; i < currentTLVsize; i += 5) {
						memcpy (currentIP4, &LLDPreceivedPayload[currentPayloadByte+i], 4);	// get IP address
						inet_ntop(AF_INET, currentIP4, currentIP4string, 20);	// convert binary representation to string
						sprintf(currentIP4string, "%s/%i, \n             ", currentIP4string, LLDPreceivedPayload[currentPayloadByte+i+4]);		// get IP address, then subNetwork
						strcat (TLVstringbuffer, currentIP4string);
					}
					
					TLVstringbuffer[strlen(TLVstringbuffer)-2-1-13] = 0;		// remove last comma and space
					
					TLV_CUSTOM_COPY( "IPv4: ", TLVstringbuffer, strlen(TLVstringbuffer));
					
				case SUBTYPE_IPV6:
					
					TLVstringbuffer[0] = 0;
					char currentIP6[16] = "";
					char currentIP6string[100] = "";
					
					for (int i = 6; i < currentTLVsize; i += 17) {
						memcpy (currentIP6, &LLDPreceivedPayload[currentPayloadByte+i], 16);	// get IP address
						inet_ntop(AF_INET6, currentIP6, currentIP6string, 100);	// convert binary representation to string
						
						sprintf(currentIP6string, "%s/%i, \n             ", currentIP6string, LLDPreceivedPayload[currentPayloadByte+i+16]);		// get IP address, then subNetwork
						strcat (TLVstringbuffer, currentIP6string);
					}
					
					TLVstringbuffer[strlen(TLVstringbuffer)-2-1-13] = 0;		// remove last comma and space
					
					TLV_CUSTOM_COPY( "IPv6: ", TLVstringbuffer, strlen(TLVstringbuffer));
					
				case SUBTYPE_EMAIL:
					TLV_STRING_COPY( "Email: ");
					
				case SUBTYPE_DHCP:
					TLV_STRING_COPY( "DHCP: ");
					
				case SUBTYPE_ROUTER:
					TLV_STRING_COPY( "Router: ");
					
				case SUBTYPE_COMBINED_STRING:
					TLV_STRING_COPY( "Combined String: ");
					
			}
			
			
			
//			printf ("*Pos: %i  	Type: %i   	Size: %i  	Subtype: %i 	Content: %s#\n", currentPayloadByte, (LLDPreceivedPayload[currentPayloadByte] >> 1), currentTLVsize, LLDPreceivedPayload[currentPayloadByte+5], parsedTLVs [numberParsedTLVs-1]);	//	, &currentTLVcontents[4]
			
		}
		
//		memset(currentTLVcontents, 0, currentTLVsize+1);
		currentPayloadByte += currentTLVsize + 2;	// + TLVheader
	}
	
	printf("\n #####Parsed TLVs: %i#####\n", numberParsedTLVs);
	for (int i = 0; i < numberParsedTLVs; i++)
		printf("%s#\n",parsedTLVs[i]);
	
	return parsedTLVs;
}
