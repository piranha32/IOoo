/*
 * tlc5946.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: jacek
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "SPI.h"
#include "GPIOoo.h"
#include "GPIOpin.h"
#include "TestTLC5946.h"
#include "debug.h"

SPI *setupSPI()
{
	SPI *spi = new SPI();
	printf("open: %d\n", spi->open(1, 0));
	//printf("setMode: %d\n", spi.setMode(SPI_CPHA));
	printf("setMode: %d\n", spi->setMode(0));
	printf("setSpeed: %d\n", spi->setSpeed(5000));

	return spi;
}

int main()
{
	SPI *spi = setupSPI();
	TestTLC5946 tlc(spi, "tlc5946_clock.bin");
	tlc.loop();
	return 0;
}

