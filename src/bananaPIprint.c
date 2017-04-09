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
	currentPIline = 1;
	
//	for (int lines = 0; lines < 14; lines++) {
	
	for (int currentTLV = 0; currentTLV < PARSED_TLVS_MAX_NUMBER; currentTLV++) {
		
		for (currentPosInTLV = 0, currentLastSpace = 0; (parsedBeaconContents[currentTLV][currentPosInTLV] != 0) && (currentPIline < 10) ; currentPosInTLV++) {
			
			if ( parsedBeaconContents[currentTLV][currentPosInTLV+1] == (int) '\n' || parsedBeaconContents[currentTLV][currentPosInTLV+1] == 0 ) {
				
				printf("Line %i:\t", currentPIline);
				
				strncpy(buf, &parsedBeaconContents[currentTLV][currentLastSpace], currentPosInTLV - currentLastSpace + 1);
				buf[currentPosInTLV - currentLastSpace + 1] = 0;
				puts(buf);
				
				#ifdef BANANAPI_SWITCH
				RAIO_SetFontSizeFactor( 0 );
				RAIO_print_text( 0, 16*currentTLV, buf, COLOR_BLACK, COLOR_WHITE );
				#endif
				
				currentLastSpace = currentPosInTLV + 2;
				
				currentPIline++; 
			}
		}
	}
	
	return;
}


