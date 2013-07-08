//============================================================================
// Name        : BeagleboneSPI.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
//#include <sys/mman.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include <math.h>
//#include <mcheck.h>
#include <pruss/prussdrv.h>
#include <pruss/pruss_intc_mapping.h>
#include <errno.h>

#include "SPI.h"
#include "GPIOoo.h"
#include "GPIOpin.h"
#include "device/HD44780gpioPhy.h"
#include "device/HD44780.h"
#include "device/TLC5946phy.h"
#include "device/TLC5946chain.h"
#include "TestTLC5946.h"
#include "TestLCD.h"
#include "TestGPIOLeds.h"
#include "debug.h"

using namespace std;

#define SEQ_LEN 10

int testSPI(SPI &spi)
{
	uint8_t txbuf[SEQ_LEN * 2];
	uint8_t rxbuf[SEQ_LEN * 2];
	for (int i = 0; i < SEQ_LEN; i++)
	{
		txbuf[i * 2] = i;
		txbuf[i * 2 + 1] = 0;
		rxbuf[i * 2] = 0;
		rxbuf[i * 2 + 1] = 0;
	}

	cout << "12 bits per word" << endl;
	int r = spi.setBitsPerWord(12);
	if (r <= 0)
		printf("spi.setBitsPerWord(12)=%i\n", r);
	r = spi.xfer1(txbuf, rxbuf, SEQ_LEN * 2);

	for (int i = 0; i < SEQ_LEN * 2; i++)
		printf("%02x ", rxbuf[i]);
	cout << endl;

	cout << "5 bits per word" << endl;
	for (int i = 0; i < SEQ_LEN; i++)
	{
		txbuf[i] = i;
		rxbuf[i] = 0;
	}
	r = spi.setBitsPerWord(5);
	if (r <= 0)
		printf("spi.setBitsPerWord(5)=%i\n", r);
	r = spi.xfer1(txbuf, rxbuf, SEQ_LEN);

	for (int i = 0; i < SEQ_LEN; i++)
		printf("%02x ", rxbuf[i]);
	cout << endl;

	return r;
}



SPI *setupSPI()
{
	SPI *spi=new SPI();
	printf("open: %d\n", spi->open(1, 0));
	//printf("setMode: %d\n", spi.setMode(SPI_CPHA));
	printf("setMode: %d\n", spi->setMode(0));
	printf("setSpeed: %d\n", spi->setSpeed(5000));

	return spi;
}


int main()
{
	//cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	//testGPIO();
	//return 0;

	TestLCD lcdTest(8);
	lcdTest.loop();
	//return 0;

	TestGPIOLeds leds();
	leds.loop(10);
	//return 0;

	SPI *spi=setupSPI();
	TestTLC5946 tlc(spi,"tlc5946_clock.bin");
	tlc.loop();


	cout << "!!!Goodbye cruel World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
