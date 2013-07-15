/*
 * JDT18003T01.cpp
 *
 *  Created on: Jul 14, 2013
 *      Author: jacek
 */

#include "device/JDT18003T01.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "debug.h"
#include <unistd.h>

JDT18003T01::JDT18003T01(SPI *spi, GPIOpin *ctrl)
{
	active = false;
	this->ctrl = ctrl;
	this->spi = spi;

	spi->setBitsPerWord(8);
	spi->setLSBFirst(true);

	if (spi == NULL || ctrl == NULL)
	{
		debug(0, "No valid control interfaces passed to JDT18003T01\n");
		return;
	}

	reset_pin = ctrl->findPinIndex((char *) "reset");
	rs_pin = ctrl->findPinIndex((char *) "rs");

	debug(2,"JDT18003T01::JDT18003T01: rs_pin:%i, reset_pin:%i",rs_pin,reset_pin);

	//setup directions on control lines
	ctrl->enableOutput(reset_pin, true);
	ctrl->enableOutput(rs_pin, true);

	active = true;
	setReset(false);

}

JDT18003T01::~JDT18003T01()
{
	debug(2, "JDT18003T01::~JDT18003T01()");
}

void JDT18003T01::setReset(uint8_t reset)
{
	if (!active)
		return;

	if (reset)
		ctrl->clearBit(reset_pin);
	else
		ctrl->setBit(reset_pin);
}

void JDT18003T01::setDataMode(bool datamode)
{
	if (!active)
		return;

	if (datamode)
		ctrl->setBit(rs_pin);
	else
		ctrl->clearBit(rs_pin);
}

int JDT18003T01::send(uint8_t wbuf[], int len)
{
	if (!active)
		return -ENODEV;

	return spi->xfer1(wbuf, NULL, len);
}
