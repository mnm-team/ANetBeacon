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
	
	int currentPosInTLV, currentPIline, currentLastSpace;
	currentPIline = 0;
	
//	for (int lines = 0; lines < 14; lines++) {
	
	for (int currentTLV = 0; currentTLV < PARSED_TLVS_MAX_NUMBER; currentTLV++) {
		
		for (currentPosInTLV = 1, currentLastSpace = 0; (parsedBeaconContents[currentTLV][currentPosInTLV-1] != 0) && (currentPIline < PARSED_TLVS_MAX_NUMBER) ; currentPosInTLV++) {
			
			if (// parsedBeaconContents[currentTLV][currentPosInTLV] == '\n' ||
				parsedBeaconContents[currentTLV][currentPosInTLV] == 0 ||
				currentPosInTLV - currentLastSpace > (currentLastSpace == 0 ? 39 : 39 - DESCRIPTOR_WIDTH)) {
				
				printf("Line %i:  \t", currentPIline);
				
				if (currentLastSpace == 0) {
					snprintf(buf, currentPosInTLV - currentLastSpace + 1, "%s", &parsedBeaconContents[currentTLV][currentLastSpace]);
				}
				else {
					snprintf(buf, currentPosInTLV - currentLastSpace + 1 + DESCRIPTOR_WIDTH, "%*s%s", DESCRIPTOR_WIDTH, "", &parsedBeaconContents[currentTLV][currentLastSpace]);
				}
				
				puts(buf);
				
				#ifdef BANANAPI_SWITCH
				RAIO_SetFontSizeFactor( 0 );
				RAIO_print_text( 0, 16*currentPIline, (unsigned char*) buf, COLOR_BLACK, COLOR_WHITE );
				#endif
				
				currentLastSpace = currentPosInTLV;
//				if (currentPosInTLV - currentLastSpace > (currentLastSpace == 0 ? 39 : 39 - DESCRIPTOR_WIDTH))	currentLastSpace--	// adjust current position in case 
				
				currentPIline++; 
			}
		}
	}
	
	return;
}


