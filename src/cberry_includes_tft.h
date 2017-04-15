/*##############################################################*/
/* 																*/
/* File		: tft.h												*/
/*																*/
/* Project	: TFT for Raspberry Pi Revision 2					*/
/* 																*/
/* Date		: 2013-11-22   	    last update: 2013-12-06			*/
/* 																*/
/* Author	: Hagen Ploog   									*/
/*		  	  Kai Gillmann										*/
/*		  	  Timo Pfander										*/
/* 																*/
/* IDE	 	: Geany 1.22										*/
/* Compiler : gcc (Debian 4.6.3-14+rpi1) 4.6.3					*/
/*																*/
/* Copyright (C) 2013 admatec GmbH								*/
/*																*/
/*																*/
/* Description  :												*/
/* 																*/
/*	This file declared functions for the SPI communications 	*/
/*	between the Raspberry Pi and the TFT and for the			*/
/*	initialization of the GPIO Pins of the Raspberry Pi.		*/
/*																*/
/*																*/
/* License:														*/
/*																*/
/*	This program is free software; you can redistribute it 		*/
/*	and/or modify it under the terms of the GNU General			*/
/*	Public License as published by the Free Software 			*/
/*	Foundation; either version 3 of the License, or 			*/
/*	(at your option) any later version. 						*/
/*    															*/
/*	This program is distributed in the hope that it will 		*/
/*	be useful, but WITHOUT ANY WARRANTY; without even the 		*/
/*	implied warranty of MERCHANTABILITY or 						*/
/*	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 		*/
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

#ifndef TFT_H
#define TFT_H

#include <stdint.h>
#include "cberry_includes_RAIO8870.h"


// define pins on connector P1 using wiringPi numbering
//#define MOSI      12
//#define MISO      13
//#define SCLK      14
#define OE        0
//#define SPI_CE1   11 // FIXME: chipselect / CE1
#define RAIO_RS   1
#define RAIO_RST  6
#define RAIO_CS   10 // FIXME: chipselect / CE0
#define RAIO_WR   5
#define RAIO_RD   4
#define RAIO_WAIT 3
#define RAIO_INT  2

//#define RAIO_WRpin 24


// declaration of a union (see RAIO8870.h)
// ----------------------------------------------------------
extern union my_union number;


// initialization of GPIO and SPI
// ----------------------------------------------------------
void TFT_init_board( void );


// hard reset of the graphic controller and the tft
// ----------------------------------------------------------
void TFT_hard_reset( void );	


// wait during raio is busy
// ----------------------------------------------------------
void TFT_wait_for_raio ( void );


// write byte to register
// ----------------------------------------------------------
void TFT_RegWrite( uint16_t reg );


// write byte to tft
// ----------------------------------------------------------
void TFT_DataWrite( uint16_t data );


// write 'count'-bytes to tft
// ----------------------------------------------------------
void TFT_DataMultiWrite( uint16_t *data, uint32_t count );

#endif

// vim:set ts=4 sw=4 noet:
