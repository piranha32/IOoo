/*
 * TLC5946.cpp
 *
 *  Created on: Jun 3, 2013
 *      Author: jacek
 */

#include "device/TLC5946chain.h"
#include <string.h>
#include <stdio.h>
#include "debug.h"

TLC5946chain::TLC5946chain(TLC5946phy *phy, int num)
{
	this->phy = phy;
	int tmp = num >> 4;
	if (num & 0xf)
		tmp++;
	chain_length = tmp * 16;
	brightness = new uint16_t[chain_length];
	memset(brightness, 0, chain_length * sizeof(brightness[0]));
	brightness_changed = true;

	//dot_num=
	dot = new uint8_t[chain_length];
	memset(dot, 0xff, chain_length * sizeof(dot[0]));
	dot_changed = true;

	commit();
}

TLC5946chain::~TLC5946chain()
{
}

void TLC5946chain::setBrightness(int i, uint16_t b)
{
	b &= 0x0fff; //only lower 12 bits are valid

	brightness[i] = b & 0xfff;
	brightness_changed = true;

}

void TLC5946chain::setDOT(int i, uint16_t dot)
{
	this->dot[i] = dot & 0x3f;
	dot_changed = true;
}

void TLC5946chain::blank(int b)
{
	phy->setBlank((b) ? 1 : 0);
}

void TLC5946chain::commit()
{
	if (dot_changed)
	{
		printf("Committing DOT\n");
		phy->setMode(1);

		/*
		 printf("DOT:\n");
		 for(int i=0;i<chain_length;i++)
		 printf("%02x ",dot[i]);
		 printf("\n");
		 */

		phy->setBitsPerWord(6);
		phy->xfer(dot, 0, chain_length);
		dot_changed = false;
	}

	if (brightness_changed)
	{
		debug(2, "Committing brightness\n");
		phy->setMode(0);

		/*
		 printf("Brightness:\n");
		 for(int i=0;i<br_num;i++)
		 printf("%02x ",brightness[i]);
		 printf("\n");
		 */

		phy->setBitsPerWord(12);
		phy->xfer((uint8_t*) brightness, 0, chain_length * 2);
		brightness_changed = false;
	}

}
