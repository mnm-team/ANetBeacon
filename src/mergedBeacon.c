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
	1. Add desired field to structure in Beacon.h
	2. Add desired options in configure.h, then configure.c
	3. Copy field contents from Configure structure to LANbeacon structure in function below
	4. Add field to Fliesstext string
	5. Add fields to size calculation
*/

// code based on https://github.com/ciil/nine-mens-morris/blob/master/src/config.c
char *mergedLANbeaconCreator (int *argc, char **argv, int *LLDPDU_len) {
	
	char *myLANbeacon = malloc(1500);
//	myLANbeacon->combinedBeacon = malloc(1500);		// old code:	malloc(sizeof(char)*(2+myLANbeacon->TLVlength));
	int currentByte = 0;	//counter for current position in Array combinedBeacon, starting after TLV header
	char *combinedString[5];	// Maximum of 5 strings of combined human-readable text in case they are longer than 507 bytes (TLV max)
	for(int i=0; i<5; i++) {combinedString[i] = malloc(507); strcpy(combinedString[i],"");}
	
	unsigned char chasisSubtype[9] = { 0x02, 0x07, 0x04, 0xbc, 0x5f, 0xf4, 0x14, 0x34, 0x6d };
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
				transferCombinedBeacon (200, "ID", myLANbeacon, &currentByte);	// putting "ID" as placeholder
				transferCombinedString ("ID: ", combinedString, optarg); 
				
				// transferring ID in machine-readable format, since it can't be trasferred as String (if ID <256, first byte would be interpreted as Null-Char/End of String)
				unsigned short int VLAN_id = (short int) strtoul(optarg,NULL,10);
				unsigned short int VLAN_id_net_byteorder = htons(VLAN_id);
				myLANbeacon[currentByte-2] = ((unsigned char *)(&VLAN_id_net_byteorder))[0];
				myLANbeacon[currentByte-1] = ((unsigned char *)(&VLAN_id_net_byteorder))[1];
				break;
				
			case 'n':	//## TLV VLAN name ##//
				transferToCombinedBeaconAndString(201, "Name: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;
				
			case 'c':
				transferToCombinedBeaconAndString(202, "Custom: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;
			
/*			case '4':
				transferCombinedString ("IPv4: ", combinedString, optarg); 
				

				transferCombinedBeacon (203, IP4address, myLANbeacon, &currentByte);

				puts("No IP addresses in correct format could be found in provided string (correct example: 192.168.178.1/24). Only raw string will be copied into summary. "); 
				
				break;
*/


			case '4':
//				printf("in  Loop: %s\n",optarg);	//debug
				
				IPparser (AF_INET, optarg, combinedString, myLANbeacon, &currentByte);
				
				break;
				
			case '6':
				
//				printf("in  Loop: %s\n",optarg);	//debug
				IPparser (AF_INET6, optarg, combinedString, myLANbeacon, &currentByte);
				
				break;

			case 'e':
				;
				regex_t compiled_regex;
				regcomp(&compiled_regex, "[a-zA-Z0-9][a-zA-Z0-9_.]+[a-zA-Z0-9]+@[a-zA-Z0-9_]+(\\.[a-zA-Z]{2,})+", REG_EXTENDED);
				if (regexec(&compiled_regex, optarg, 0, NULL, 0) == REG_NOMATCH) {puts("Es gibt einen Fehler in der angegebenen Email-Adresse.");}
				regfree(&compiled_regex);
				
				transferToCombinedBeaconAndString(205, "Email: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case 'd':
				transferToCombinedBeaconAndString(206, "DHCP: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case 'r':
				transferToCombinedBeaconAndString(207, "Router: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;

			case 'h':
				printHelp();
				break;
				
			default:
				printHelp();
		}
	}
	
	for(int i = 0; i < 5; i++) {
		if (0 < strlen(combinedString[i])) { transferCombinedBeacon(217, combinedString [i], myLANbeacon, &currentByte); }
	}
	
	*LLDPDU_len = currentByte;
	
	return myLANbeacon;
}





void transferToCombinedBeaconAndString (unsigned char subtype, char *TLVdescription, char **combinedString, char *source, char *combinedBeacon, int *currentByte) {
	transferCombinedBeacon (subtype, source, combinedBeacon, currentByte);
	if (!(combinedString == NULL))  transferCombinedString (TLVdescription, combinedString, source); 
}

void transferCombinedBeacon (unsigned char subtype, char *source, char *combinedBeacon, int *currentByte) {
	//## calculating TLV length without header, then combining TLV Header and transfering combined TLV Header to combined Beacon ##//
	unsigned short int currentTLVlength = strlen(source);
	unsigned char TLVtype = 127;
//qqq	printf ("%i", currentTLVlength + *currentByte + 6);
	if (1500 < (currentTLVlength + *currentByte + 6)) {puts("Maximum of 1500 Bytes in LLDP-Packet exceeded, not all information will be transmitted. Please include less information."); return;}
	if (currentTLVlength > 507) {printf("Warning, TLV is %i Bytes long, exceeding maximum of 507 characters in String. End of string will be cut off.", currentTLVlength); currentTLVlength = 507;}
	
	unsigned short int TLVheader_combined = (TLVtype * 0b1000000000) | (currentTLVlength+4);	// Shift der bits nach Rechts und anschließendes bitweises OR zur Kombination der 7+9 bit
	combinedBeacon[*currentByte+0] = ((unsigned char *)(&TLVheader_combined))[1];
	combinedBeacon[*currentByte+1] = ((unsigned char *)(&TLVheader_combined))[0];
	
	// transfer OUI and OUI subtype to combined Beacon
	combinedBeacon[*currentByte+2] = 'L' | 0b10000000;	// WARNING: first two bits 11 have to be left like this		REF: nach http://standards.ieee.org/getieee802/download/802-2014.pdf clause 9.3
	combinedBeacon[*currentByte+3] = 'M' ;
	combinedBeacon[*currentByte+4] = 'U' ;
	combinedBeacon[*currentByte+5] = subtype;
	
	// transfer information to combinedBeacon
	memcpy(&combinedBeacon[*currentByte+6], source, currentTLVlength);
	*currentByte = *currentByte + 6 + currentTLVlength;
}

void transferCombinedString (char *TLVdescription, char **combinedString, char *source) {
	// transfer string to combinedString
	int stringToBeFilled = 0;
	if (507 < (strlen(TLVdescription) + strlen(source) + 2 ) ) { printf("String: %s is too long to be included as text. will be skipped\n", source); return;}
	for( ; stringToBeFilled < 5; stringToBeFilled++) {
		if (507 > (strlen(combinedString[stringToBeFilled]) + strlen(TLVdescription) + strlen(source) + 2 ) ) { break; }
	}
	
	strcat(combinedString[stringToBeFilled], TLVdescription);
	strcat(combinedString[stringToBeFilled], source);
	strcat(combinedString[stringToBeFilled], ". ");
}




void IPparser (int IPv_4or6, char *optarg, char **combinedString, char *myLANbeacon, int *currentByte) {
	
	int IP_binlen;
	size_t IP_strlen; 
	if (IPv_4or6 == AF_INET) {IP_strlen = INET_ADDRSTRLEN; IP_binlen = 5;}
	else if (IPv_4or6 == AF_INET6) {IP_strlen = INET6_ADDRSTRLEN; IP_binlen = 17;}
	char gefundeneAdressenStrings[10][2][IP_strlen];
	char bufferForLANbeacon[200] = "";
	
//	printf("out Loop: %s\n",optarg);							//debug
//	printf("strlen: %zu   ip: %i\n", IP_strlen, IPv_4or6);		//debug
	
	regex_t compiled_regex;
	int regex_return;
	size_t regex_number_matches = 7;
	regmatch_t regex_matches_pointer[7];
	
	int gefundeneIPAdressenAnzahl=0;
	regex_matches_pointer[0].rm_so = 0;
	regex_matches_pointer[0].rm_eo = 0;
	
	if (IPv_4or6 == AF_INET) 		regex_return = regcomp(&compiled_regex, "([0-9.]{7,15})\\/([0-9]{1,2})", REG_EXTENDED);
	else if (IPv_4or6 == AF_INET6)	regex_return = regcomp(&compiled_regex, "([a-fA-F0-9:]{4,45})\\/([0-9]{1,3})", REG_EXTENDED);
	
//	printf ("%i\n",regex_return);	//debug
	
	int endOfLastString = 0;
	for (int i=0;i<10;i++) {	// look for max 10 addresses
		endOfLastString += regex_matches_pointer[0].rm_eo;
		regex_return = regexec(&compiled_regex, &optarg[endOfLastString], regex_number_matches, regex_matches_pointer, 0);
		
		if (!regex_return) {
			for (int j = 1; j<=2; j++) {	// get entire IP and subnetwork
				memcpy (gefundeneAdressenStrings[i][j], "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", IP_strlen);
				strncpy (gefundeneAdressenStrings[i][j], &optarg[endOfLastString+regex_matches_pointer[j].rm_so], regex_matches_pointer[j].rm_eo - regex_matches_pointer[j].rm_so); 
				
//				printf ("xxx   %s\n", gefundeneAdressenStrings[i][j]);		//debug
			}
			gefundeneIPAdressenAnzahl++;
			
			if (IPv_4or6 == AF_INET) strcat(bufferForLANbeacon, "#####");	// Buffer string to reserve space for binary representation in LANbeacon
			else if (IPv_4or6 == AF_INET6)	strcat(bufferForLANbeacon, "#################");
		}
//		else if (regex_return == REG_NOMATCH) { break; }
	}
	
	if (IPv_4or6 == AF_INET) {
		transferCombinedString ("IPv4: ", combinedString, optarg); 
		transferCombinedBeacon (203, bufferForLANbeacon, myLANbeacon, currentByte);
	}
	else if (IPv_4or6 == AF_INET6) {
		transferCombinedString ("IPv6: ", combinedString, optarg); 
		transferCombinedBeacon (204, bufferForLANbeacon, myLANbeacon, currentByte);
	}


	regfree(&compiled_regex);

	//## transfer found addresses to combined beacon in binary format
	if (1 > gefundeneIPAdressenAnzahl) {
		puts("No IP addresses in correct format could be found in provided string (correct example: 192.168.178.1/24). Only raw string will be copied into summary. "); 
		return;
	}
	
	unsigned char *IP4address; 
	if (IPv_4or6 == AF_INET) IP4address = malloc(IP_binlen);	// Buffer string to reserve space for binary representation in LANbeacon
	else if (IPv_4or6 == AF_INET6)	IP4address = malloc(IP_binlen);
	
	struct in_addr result;
	int IPcurrentByte = *currentByte - gefundeneIPAdressenAnzahl*IP_binlen; 
	
	for (int i = 0; i < gefundeneIPAdressenAnzahl; i++) {
	
		if (inet_pton(IPv_4or6, gefundeneAdressenStrings[i][1], IP4address) == 1) {
	/*debug		printf("%u\n",IP4address[0]); // success
			printf("%u\n",IP4address[1]); // success
			printf("%u\n",IP4address[2]); // success
			printf("%u\n",IP4address[3]); // success
			printf("%u\n",IP4address[4]); // success
	*/	}
		else {
			printf("Error Parsing IP4-address: %s\n", optarg);
		}					
		
		for (int j = 0; j < IP_binlen-1; j++) {
//						printf("%s ", gefundeneAdressenStrings[i][j]);	//debug
			myLANbeacon[IPcurrentByte++] = IP4address[j];
		}
		
		myLANbeacon[IPcurrentByte++] = (unsigned char) strtoul(gefundeneAdressenStrings[i][2],NULL,10);	// set subnet byte
		
//		puts("");	//debug
	}
//	printf ("after loop matchup between currentbyte counters %i %i\n", IPcurrentByte, *currentByte);	//debug
//	printf ("%i\n", gefundeneIPAdressenAnzahl);		//debug
	
//				transferToCombinedBeaconAndString(203, "IPv4: ", combinedString, optarg, myLANbeacon, &currentByte);
	return;
}





void printHelp() {
	printf("Usage: \t./LANbeacon [-i VLAN_ID] [-n VLAN_NAME] [-4 IPv4_SUBNETWORK (e.g. 192.168.178.133/24)] [-6 IPv6_SUBNETWORK] [-e EMAIL_CONTACTPERSON] [-d DHCP_TYPES] [-r ROUTER_INFORMATION] [-c CUSTOM_STRING]\n");
	printf("\t./client -h\n");
	exit(EXIT_FAILURE);
}


