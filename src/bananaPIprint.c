#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "tft.h"
#include "RAIO8870.h"
#include "define.h"



void bananaPIprint (char **parsedBeaconContents) {
	
	#ifdef BANANAPI_SWITCH
	TFT_init_board();
	TFT_hard_reset();
	RAIO_init();
	RAIO_clear_screen();
	#endif
	
puts("\n\n\n####PIdisplay: ####");	
	char buf[800];
	int c = 0;
	int column;
	
	int currentPosInTLV, currentPIline, endOfLastPartialString;
	
	
	int currentLastSpace = 0;
	
	while (1) {
		
		currentPIline = 0;
		
		printf("\e[1;1H\e[2J");
		
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
				
					printf("Line %i:  \t", currentPIline);
				
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
						sleep (2);
						currentPIline = 0;
						
						printf("\e[1;1H\e[2J");
					
						#ifdef BANANAPI_SWITCH
						RAIO_clear_screen();
						#endif
					} 
				}
			}
		}
	
		if (currentPIline != 0)	sleep (2);	//if sleep (2) already has been executed, don't execute again
	}
	
	return;
}


