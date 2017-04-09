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
	
	int currentLastSpace;
	
	for (int lines = 0; lines < 14; lines++) {
		
		for (int currentPosInTLV = 0, currentLastSpace = 0; parsedBeaconContents[lines][currentPosInTLV] != 0 ; currentPosInTLV++) {
			
			if ( parsedBeaconContents[lines][currentPosInTLV+1] == (int) '\n' || parsedBeaconContents[lines][currentPosInTLV+1] == 0 ) {
				
				strncpy(buf, &parsedBeaconContents[lines][currentLastSpace], currentPosInTLV - currentLastSpace + 1);
				buf[currentPosInTLV - currentLastSpace + 1] = 0;
				puts(buf);
				
				#ifdef BANANAPI_SWITCH
				RAIO_SetFontSizeFactor( 0 );
				RAIO_print_text( 0, 16*lines, (unsigned char *) buf, COLOR_BLACK, COLOR_WHITE );
				#endif
				
				currentLastSpace = currentPosInTLV;
			}
		}
	}
	
	return;
}


