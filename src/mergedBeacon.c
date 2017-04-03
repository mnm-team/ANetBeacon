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
char *mergedLANbeaconCreator (int *argc, char **argv) {
	
	char *myLANbeacon = malloc(1500);
//	myLANbeacon->combinedBeacon = malloc(1500);		// old code:	malloc(sizeof(char)*(2+myLANbeacon->TLVlength));
	int currentByte = 0;	//counter for current position in Array combinedBeacon, starting after TLV header
	char *combinedString[5];
	regex_t compiled_regex;
	for(int i=0; i<5; i++) {combinedString[i] = malloc(507); strcpy(combinedString[i],"");}
	
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
				;
				char currentIP4Octet[] = "\0\0\0\0\0\0";
				
				size_t IP4nmatch = 7;
				regmatch_t IP4pmatch[7];
				int gefundeneIP4AdressenAnzahl=0;
				int IP4reti;
				IP4pmatch[0].rm_so = 0;
				IP4pmatch[0].rm_eo = 0;
				IP4reti = regcomp(&compiled_regex, "([0-9.]{7,15})\\/([0-9]{1,2})", REG_EXTENDED);
				printf ("%i\n",IP4reti);
				char gefundeneAdressenStrings[10][2][20];
				char bufferForLANbeacon[50] = "";
				
//				printf ("%zu\n", IP4nmatch);
				
				int endOfLastString = 0;
				for (int i=0;i<10;i++) {
					endOfLastString += IP4pmatch[0].rm_eo;
					IP4reti = regexec(&compiled_regex, &optarg[endOfLastString], IP4nmatch, IP4pmatch, 0);
					if (!IP4reti) {
//						strncpy (gefundeneAdressenStrings[gefundeneIP4AdressenAnzahl], &optarg[endOfLastString+IP4pmatch[0].rm_so], 18); 
						
						for (int j = 1; j<=2; j++) {
							strncpy (gefundeneAdressenStrings[i][j], "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 20);
//							printf ("current octett before assignment: %s\n", currentIP4Octet);
//							printf ("size: %i\n",IP4pmatch[j].rm_eo - IP4pmatch[j].rm_so);
							strncpy (gefundeneAdressenStrings[i][j], &optarg[endOfLastString+IP4pmatch[j].rm_so], IP4pmatch[j].rm_eo - IP4pmatch[j].rm_so); 
//							printf ("current octett after assignment: %s\n", currentIP4Octet);
							
							printf ("xxx   %s\n", gefundeneAdressenStrings[i][j]);
						}
						
//						gefundeneAdressenStrings[gefundeneIP4AdressenAnzahl++][18] = 0;
						gefundeneIP4AdressenAnzahl++;
						strcat(bufferForLANbeacon, "#####");	// Buffer string to reserve space for binary representation in LANbeacon
					}
					else if (IP4reti == REG_NOMATCH) { break; }
				}
				transferCombinedString ("IPv4: ", combinedString, optarg); 
				transferCombinedBeacon (203, bufferForLANbeacon, myLANbeacon, &currentByte);
				regfree(&compiled_regex);

				//## transfer found addresses to combined beacon in binary format
				if (1 > gefundeneIP4AdressenAnzahl) {
					puts("No IP addresses in correct format could be found in provided string (correct example: 192.168.178.1/24). Only raw string will be copied into summary. "); 
					break;
				}
				
				
				unsigned char IP4address[10]; // = &result;
				struct in_addr result;
				int IP4currentByte = currentByte - gefundeneIP4AdressenAnzahl*5;
				
				for (int i = 0; i < gefundeneIP4AdressenAnzahl; i++) {
				
					if (inet_pton(AF_INET, gefundeneAdressenStrings[i][1], IP4address) == 1) {
						printf("%u\n",IP4address[0]); // success
					}
					else {
						printf("Error Parsing IP4-address: %s\n", optarg);
					}					
					
					for (int j = 0; j<4; j++) {
//						printf("%s ", gefundeneAdressenStrings[i][j]);
						myLANbeacon[IP4currentByte++] = IP4address[j];
					}
					
					myLANbeacon[IP4currentByte++] = (unsigned char) strtoul(gefundeneAdressenStrings[i][2],NULL,10);
					
					puts("");
				}
				printf ("%i %i\n", IP4currentByte, currentByte);
				printf ("%i\n", gefundeneIP4AdressenAnzahl);
				
//				transferToCombinedBeaconAndString(203, "IPv4: ", combinedString, optarg, myLANbeacon, &currentByte);
				break;


			case '6':
				transferToCombinedBeaconAndString(204, "IPv6: ", combinedString, optarg, myLANbeacon, &currentByte);
/*
				const char *ipstr = "::2";
				struct in_addr result;
				if (inet_pton(AF_INET6, ip6str, &result) == 1) // success! {
					
				}
*/

				break;

			case 'e':
				;
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
	
	FILE *combined = fopen("testNewTransfer","w");
	fwrite(myLANbeacon, currentByte, 1, combined);		// Two additional bytes for header (TLVtype and TLVlength)
	
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
	strncpy(&combinedBeacon[*currentByte+6], source, currentTLVlength);
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


void printHelp() {
	printf("Usage: \t./LANbeacon [-i VLAN_ID] [-n VLAN_NAME] [-4 IPv4_SUBNETWORK (e.g. 192.168.178.133/24)] [-6 IPv6_SUBNETWORK] [-e EMAIL_CONTACTPERSON] [-d DHCP_TYPES] [-r ROUTER_INFORMATION] [-c CUSTOM_STRING]\n");
	printf("\t./client -h\n");
	exit(EXIT_FAILURE);
}


