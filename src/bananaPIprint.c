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
	
		
	char buf[256];
	int c = 0;
	int column;
	
	for (int lines = 0; lines < 14; lines++) {
		
		strncpy(buf, parsedBeaconContents[lines],40);
		
		for (column = 0; column < 40; column++) {
/*			c = fgetc(  stdin );
			if ( c == EOF || c == (int) '\n') break;
			buf[column] = (char) c;
*/			
//			buf[column] = '#';
			
		}
		
		buf[column] = 0;
		puts(buf);

		#ifdef BANANAPI_SWITCH
		RAIO_SetFontSizeFactor( 0 );
//		RAIO_print_text( 0, 16*lines, (unsigned char *) buf, COLOR_BLACK, COLOR_WHITE );
		#endif

		
	}
	
	return;
}


