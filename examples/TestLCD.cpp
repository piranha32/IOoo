/*
 * TestLCD.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: jacek
 */

#include <stdio.h>
#include "TestLCD.h"
#include "debug.h"

TestLCD::TestLCD(int bits)
{
	gp = GPIOoo::getInstance();

	char **lcdPins;
	char **lcdNames;

	const char *lcdPins4[] =
	//               RS       E      D0      D1      D2      D3
				{ "P9_11", "P9_12", "P9_16", "P9_17", "P9_18", "P9_21" };
	const char *lcdNames4[] =
		{ "RS", "E", "D[0]", "D[1]", "D[2]", "D[3]" };

	const char *lcdPins8[] =
	//              RS        E        D0       D1       D2       D3       D4       D5       D6       D7
				{ "P9_11", "P9_12", "P9_16", "P9_17", "P9_18", "P9_21", "P9_24",
						"P9_25", "P9_26", "P9_27" };
	const char *lcdNames8[] =
		{ "RS", "E", "D[0]", "D[1]", "D[2]", "D[3]", "D[4]", "D[5]", "D[6]",
				"D[7]" };

	if (bits == 4)
	{
		lcdPins = (char **) lcdPins4;
		lcdNames = (char **) lcdNames4;
	}
	else if (bits == 8)
	{
		lcdPins = (char **) lcdPins8;
		lcdNames = (char **) lcdNames8;
	}
	else
	{
		iooo_debug(0, "Incorrect number of bits in LCD interface");
		return;
	}

	printf("Creating blockLCD with %i data bus\n", bits);
	blockLCD = gp->claim((char **) lcdPins, bits + 2);

	printf("Naming pins\n");
	blockLCD->namePins((char **) lcdNames);

	printf("Creating HD44780gpioPhy\n");
	lcdPhy = new HD44780gpioPhy(blockLCD);
	printf("Creating HD44780\n");
	lcd = new HD44780(lcdPhy, 20, 4);

	printf("Initializing LCD\n");
	lcd->init();
	lcd->clear();

}

TestLCD::~TestLCD()
{
	delete lcd;
	delete lcdPhy;
	delete blockLCD;
}

void TestLCD::loop()
{
	printf("Action!\n");
	for (int i = 1; i <= 10; i++)
	{
		char buf[40];
		sprintf(buf, (const char*) "Test line %i\n", i);
		lcd->print(buf);
	}

}
