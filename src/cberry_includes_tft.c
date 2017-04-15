/*##############################################################*/
/*																*/
/* File :	tft.c												*/
/*																*/
/* Project :	TFT for Raspberry Pi Revision 2					*/
/*																*/
/* Date	: 2013-11-22       last update: 2013-12-06				*/
/*																*/
/* Author	: Hagen Ploog										*/
/*		  Kai Gillmann											*/
/*		  Timo Pfander											*/
/*																*/
/* IDE		: Geany 1.22										*/
/* Compiler : gcc (Debian 4.6.3-14+rpi1) 4.6.3					*/
/*																*/
/* Copyright (C) 2013 admatec GmbH								*/
/*																*/
/*																*/
/* Description	:												*/
/*																*/
/*	This file controls the communications between the 			*/
/*	Raspberry Pi and the TFT. The file initialized also the		*/
/*	GPIO Pins of the Raspberry Pi.								*/
/*																*/
/*																*/
/* License:														*/
/*																*/
/*	This program is free software; you can redistribute it 		*/
/*	and/or modify it under the terms of the GNU General			*/
/*	Public License as published by the Free Software 			*/
/*	Foundation; either version 3 of the License, or 			*/
/*	(at your option) any later version. 						*/
/*																*/
/*	This program is distributed in the hope that it will 		*/
/*	be useful, but WITHOUT ANY WARRANTY; without even the 		*/
/*	implied warranty of MERCHANTABILITY or 						*/
/*	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General	 	*/
/*	Public License for more details. 							*/
/*																*/
/*	You should have received a copy of the GNU General 			*/
/*	Public License along with this program; if not, 			*/
/*	see <http://www.gnu.org/licenses/>.							*/
/*																*/
/*																*/
/* Revision History:											*/
/*																*/
/*	Version 1.0 - Initial release								*/
/*																*/
/*																*/
/*																*/
/*##############################################################*/


#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "cberry_includes_RAIO8870.h"
#include "cberry_includes_tft.h"

int spi_fd;

// initialization of GPIO and SPI
// ----------------------------------------------------------
void TFT_init_board ( void )
{

	wiringPiSetup();
	// spi: speed 125MHz, mode0, cs1 polarity low, msb first
	//spi_fd = wiringPiSPISetup (1, 80000000 );
	spi_fd = wiringPiSPISetup (1, 50000000 );

	// *************** output pins

	pinMode( OE, OUTPUT );
	digitalWrite( OE, HIGH );

	pinMode( RAIO_RST, OUTPUT );
	digitalWrite( RAIO_RST, HIGH );

	pinMode( RAIO_CS, OUTPUT );
	digitalWrite( RAIO_CS, HIGH );

	pinMode( RAIO_RS, OUTPUT );
	digitalWrite( RAIO_RS, HIGH );

	pinMode( RAIO_WR, OUTPUT );
	digitalWrite( RAIO_WR, HIGH );

	pinMode( RAIO_RD, OUTPUT );
	digitalWrite( RAIO_RD, HIGH );


	// *************** input pins

	pinMode( RAIO_WAIT, INPUT );
	pullUpDnControl( RAIO_WAIT, PUD_UP);

	pinMode( RAIO_INT, INPUT );
	pullUpDnControl( RAIO_INT, PUD_UP);

}


// hard reset of the graphic controller and the tft
// ----------------------------------------------------------
void TFT_hard_reset( void )
{
	digitalWrite( RAIO_RST, LOW );
	delay( 10 );
 	digitalWrite( RAIO_RST, HIGH );
 	delay( 1 );
}


// wait during raio is busy
// ----------------------------------------------------------
void TFT_wait_for_raio ( void )
{
	while ( !digitalRead( RAIO_WAIT ) );
}


// write data via SPI to tft
// ----------------------------------------------------------
void TFT_SPI_data_out ( uint16_t data )
{
	union my_union number;
	char buffer[2];

	number.value = data;
	buffer[0] = (char) number.split.high;
	buffer[1] = (char) number.split.low;

	wiringPiSPIDataRW( 1, &buffer[0], 2 );
}


// write byte to register
// ----------------------------------------------------------
void TFT_RegWrite( uint16_t reg )
{
	digitalWrite( RAIO_RS, HIGH );
	digitalWrite( RAIO_CS, LOW );
	digitalWrite( RAIO_WR, LOW );
	digitalWrite( OE, LOW );

	TFT_SPI_data_out( reg );

	digitalWrite( RAIO_WR, HIGH );
	digitalWrite( RAIO_CS, HIGH );
	digitalWrite( OE, HIGH );
}


// write byte to tft
// ----------------------------------------------------------
void TFT_DataWrite( uint16_t data )
{
	digitalWrite( RAIO_RS, LOW );
	digitalWrite( RAIO_CS, LOW );
	digitalWrite( RAIO_WR, LOW );
	digitalWrite( OE, LOW );

	TFT_SPI_data_out( data );

	digitalWrite( RAIO_WR, HIGH );
	digitalWrite( RAIO_CS, HIGH );
	digitalWrite( OE, HIGH );
};


// write 'count'-bytes to tft
// ----------------------------------------------------------
void TFT_DataMultiWrite( uint16_t *data, uint32_t count )
{
	uint32_t i;

	digitalWrite( RAIO_RS, LOW );
	digitalWrite( RAIO_CS, LOW );
	digitalWrite( OE, LOW );

	for( i=0; i<count; i++ )
	{
		// WR = 0
		digitalWrite( RAIO_WR, LOW );

		// write 2 bytes
		TFT_SPI_data_out( data[i] );

		// WR = 1
		digitalWrite( RAIO_WR, HIGH );
	}

	digitalWrite( RAIO_CS, HIGH );
	digitalWrite( OE, HIGH );

}

// vim:set ts=4 sw=4 noet:
