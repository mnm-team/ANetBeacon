#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <regex.h>

#include "beacon.h"
#include "tools.h"
#include "config.h"
#include "mergedBeacon.h"

/* Howto adding new fields:
	1. Add defines for desired new field in Beacon.h
	2. Add desired options in mergedLANbeaconCreator()
*/

// code loosely based on code from my Systempraktikum https://github.com/ciil/nine-mens-morris/blob/master/src/config.c
char *mergedLANbeaconCreator (int *argc, char **argv, int *LLDPDU_len) {
	
	char *myLANbeacon = malloc(1500);
	int currentByte = 0;	//counter for current position in Array combinedBeacon, starting after TLV header
	char *combinedString[5];	// Maximum of 5 strings of combined human-readable text in case they are longer than 507 bytes (TLV max)
	
	for(int i=0; i<5; i++) {combinedString[i] = malloc(507); strcpy(combinedString[i],"");}
	
	unsigned char chasisSubtype[9] = { 0x02, 0x07, 0x04, 0xbc, 0x5f, 0xf4, 0x14, 0x34, 0x6d };	//TODO
	memcpy(&myLANbeacon[currentByte], chasisSubtype, 9);
	currentByte += 9;
	unsigned char PortSubtype[9] = { 0x04, 0x07, 0x03, 0xbc, 0x5f, 0xf4, 0x14, 0x34, 0x6d };
	memcpy(&myLANbeacon[currentByte], PortSubtype, 9);
	currentByte += 9;
	unsigned char TimeToLive[9] = { 0x06, 0x02, 0x00, 0x14 };
	memcpy(&myLANbeacon[currentByte], TimeToLive, 4);
	currentByte += 4;
	
	//## custom TLV arguments ##//
	if(*argc == 1) printHelp();
	int opt;
	while((opt=getopt(*argc, argv, "i:n:c:4:6:e:d:r:h")) != -1) {
		switch(opt) {
			
			case 'i':	//## TLV VLAN ID ##//
				transferCombinedBeacon (SUBTYPE_VLAN_ID, "xx", myLANbeacon, &currentByte);	// putting "xx" as placeholder
				unsigned short int VLAN_id = htons( (unsigned short int) strtoul(optarg,NULL,10) );
				memcpy(&myLANbeacon[currentByte-2], &VLAN_id, 2);

				transferCombinedString ("VLAN-ID: ", combinedString, optarg); 
				break;
				
			case 'n':
				transferToCombinedBeaconAndString(SUBTYPE_NAME, "VLAN-Name: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;
				
			case 'c':
				transferToCombinedBeaconAndString(SUBTYPE_CUSTOM, "Custom-Text: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case '4':
				IPparser (AF_INET, optarg, combinedString, myLANbeacon, &currentByte);
				break;
				
			case '6':
				IPparser (AF_INET6, optarg, combinedString, myLANbeacon, &currentByte);
				break;

			case 'e':
				;
				regex_t compiled_regex;
				regcomp(&compiled_regex, "[a-zA-Z0-9][a-zA-Z0-9_.]+[a-zA-Z0-9]+@[a-zA-Z0-9_]+(\\.[a-zA-Z]{2,})+", REG_EXTENDED);
				if (regexec(&compiled_regex, optarg, 0, NULL, 0) == REG_NOMATCH) {puts("Es gibt einen Fehler in der angegebenen Email-Adresse.");}
				regfree(&compiled_regex);
				
				transferToCombinedBeaconAndString(SUBTYPE_EMAIL, "Email: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case 'd':
				transferToCombinedBeaconAndString(SUBTYPE_DHCP, "DHCP: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case 'r':
				transferToCombinedBeaconAndString(SUBTYPE_ROUTER, "Router: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case 'h':
				printHelp();
				break;
				
			default:
				printHelp();
		}
	}
	
	//## transfer combined strings to TLVs, if one combined string exceeds 507 byte limit of TLV it will be in the next part of the array ##// 
	for(int i = 0; i < 5; i++) {
		if (0 < strlen(combinedString[i]))
			transferCombinedBeacon(SUBTYPE_COMBINED_STRING, combinedString [i], myLANbeacon, &currentByte);
	}
	
	*LLDPDU_len = currentByte;
	
	return myLANbeacon;
}




//## shortcut for cases in which only a string is transferred ##//
void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, char **combinedString, char *source, char *combinedBeacon, int *currentByte) {
	transferCombinedBeacon (subtype, source, combinedBeacon, currentByte);
	if (!(combinedString == NULL))  transferCombinedString (TLVdescription, combinedString, source); 
}

//## transferring the content of the field to the combined LANbeacon in binary format ##//
void transferCombinedBeacon (unsigned char subtype, char *source, char *combinedBeacon, int *currentByte) {
	//## calculating TLV length without header, then combining TLV Header and transfering combined TLV Header to combined Beacon ##//
	unsigned short int currentTLVlength = strlen(source);
	unsigned char TLVtype = 127;
	
	if (1500 < (currentTLVlength + *currentByte + 6)) {puts("Maximum of 1500 Bytes in LLDP-Packet exceeded, not all information will be transmitted. Please include less information."); return;}
	if (currentTLVlength > 507) {printf("Warning, TLV is %i Bytes long, exceeding maximum of 507 characters in String. End of string will be cut off.", currentTLVlength); currentTLVlength = 507;}
	
	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit für subtype und Länge
	unsigned short int TLVheader_combined = htons( (TLVtype * 0b1000000000) | (currentTLVlength+4) );	
	memcpy (&combinedBeacon[*currentByte], &TLVheader_combined, 2);
	
	// transfer OUI and OUI subtype to combined Beacon
	combinedBeacon[*currentByte+2] = 'L' | 0b10000000;	// WARNING: first two bits 11 have to be left like this		REF: nach http://standards.ieee.org/getieee802/download/802-2014.pdf clause 9.3
	combinedBeacon[*currentByte+3] = 'M' ;
	combinedBeacon[*currentByte+4] = 'U' ;
	combinedBeacon[*currentByte+5] = subtype;
	
	// transfer information to combinedBeacon
	memcpy(&combinedBeacon[*currentByte+6], source, currentTLVlength);
	*currentByte = *currentByte + 6 + currentTLVlength;
}

//## transferring the content of the field to the combined string in human-readable format, if one combined string exceeds 507 byte limit of TLV it is put to the next combined string TLV ##//
void transferCombinedString (char *TLVdescription, char **combinedString, char *TLVcontents) {
	int stringToBeFilled;
	if (507 < (strlen(TLVdescription) + strlen(TLVcontents) + 2 ) ) { printf("String: %s is too long to be included as text. will be skipped\n", TLVcontents); return;}
	for(stringToBeFilled = 0; stringToBeFilled < 5; stringToBeFilled++) {
		if (507 > (strlen(combinedString[stringToBeFilled]) + strlen(TLVdescription) + strlen(TLVcontents) + 2 ) ) { break; }
	}
	
	strcat(combinedString[stringToBeFilled], TLVdescription);
	strcat(combinedString[stringToBeFilled], TLVcontents);
	strcat(combinedString[stringToBeFilled], ". ");
}




//## using regex to get IP-addresses from string input, then convert them to binary representation for transport ##//
void IPparser (int IPv_4or6, char *optarg, char **combinedString, char *myLANbeacon, int *currentByte) {
	
	int IP_binlen;
	size_t IP_strlen; 
	if (IPv_4or6 == AF_INET) {IP_strlen = INET_ADDRSTRLEN; IP_binlen = 5;}
	else if (IPv_4or6 == AF_INET6) {IP_strlen = INET6_ADDRSTRLEN; IP_binlen = 17;}
	char gefundeneAdressenStrings[10][2][IP_strlen];	// Max of 10 IPs, second field for subnet regex
	char bufferForLANbeacon[200] = "";
	
	//## using regex to get IP-addresses from string input ##//
	
	regex_t compiled_regex;
	regmatch_t regex_matches_pointer[3];
	
	if (IPv_4or6 == AF_INET) 		regcomp(&compiled_regex, "([0-9.]{7,15})\\/([0-9]{1,2})", REG_EXTENDED);
	else if (IPv_4or6 == AF_INET6)	regcomp(&compiled_regex, "([a-fA-F0-9:]{4,45})\\/([0-9]{1,3})", REG_EXTENDED);
	
	// initializing to 0 to support first run of loop, since it expects some results from previous go-through 
	int gefundeneIPAdressenAnzahl=0;
	int endOfLastString = 0;
	regex_matches_pointer[0].rm_so = 0;
	regex_matches_pointer[0].rm_eo = 0;
	
	for (int i=0;i<10;i++) {	// look for max 10 addresses
		endOfLastString += regex_matches_pointer[0].rm_eo;
		
		if (!regexec(&compiled_regex, &optarg[endOfLastString], 3, regex_matches_pointer, 0)) {
			for (int j = 1; j<=2; j++) {	// get entire IP and subnetwork
				memset(gefundeneAdressenStrings[i][j], 0, IP_strlen);
				strncpy (gefundeneAdressenStrings[i][j], &optarg[endOfLastString+regex_matches_pointer[j].rm_so], regex_matches_pointer[j].rm_eo - regex_matches_pointer[j].rm_so); 
			}
			gefundeneIPAdressenAnzahl++;
			
			if (IPv_4or6 == AF_INET) strcat(bufferForLANbeacon, "#####");	// Buffer string to reserve space for binary representation in LANbeacon
			else if (IPv_4or6 == AF_INET6)	strcat(bufferForLANbeacon, "#################");
		}
	}
	
	regfree(&compiled_regex);
	
	if (IPv_4or6 == AF_INET) {
		transferCombinedString ("IPv4: ", combinedString, optarg); 
		transferCombinedBeacon (SUBTYPE_IPV4, bufferForLANbeacon, myLANbeacon, currentByte);
	}
	else if (IPv_4or6 == AF_INET6) {
		transferCombinedString ("IPv6: ", combinedString, optarg); 
		transferCombinedBeacon (SUBTYPE_IPV6, bufferForLANbeacon, myLANbeacon, currentByte);
	}
	
	if (1 > gefundeneIPAdressenAnzahl) {
		puts("No IP addresses in correct format could be found in provided string (correct example: 192.168.178.1/24). Only raw string will be copied into summary. "); 
		return;
	}
	
	//## convert and transfer found addresses to combined beacon in binary format ##//
	
	unsigned char *IP4address = malloc(IP_binlen);	// Buffer string to reserve space for binary representation in LANbeacon
	struct in_addr result;
	int IPcurrentByte = *currentByte - gefundeneIPAdressenAnzahl*IP_binlen; 
	
	for (int i = 0; i < gefundeneIPAdressenAnzahl; i++) {
		if (inet_pton(IPv_4or6, gefundeneAdressenStrings[i][1], IP4address) != 1) 
			printf("Error Parsing IP-address: %s\n", optarg);

		for (int j = 0; j < IP_binlen-1; j++) 
			myLANbeacon[IPcurrentByte++] = IP4address[j];
		
		myLANbeacon[IPcurrentByte++] = (unsigned char) strtoul(gefundeneAdressenStrings[i][2],NULL,10);	// put subnet as 5th byte of 5-tuple
	}
	
	return;
}


void printHelp() {
	printf("Usage: \t./LANbeacon [-i VLAN_ID] [-n VLAN_NAME] [-4 IPv4_SUBNETWORK (e.g. 192.168.178.133/24)] [-6 IPv6_SUBNETWORK] [-e EMAIL_CONTACTPERSON] [-d DHCP_TYPES] [-r ROUTER_INFORMATION] [-c CUSTOM_STRING]\n");
	printf("\t./client -r\n");
	printf("\t./client -h\n");
	exit(EXIT_SUCCESS);
}


