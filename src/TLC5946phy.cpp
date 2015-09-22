/*
 * TLC5946phy.cpp
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#include "device/TLC5946phy.h"
#include <errno.h>
#include <stdio.h>
#include <pruss/prussdrv.h>
#include <pruss/pruss_intc_mapping.h>
#include "debug.h"

TLC5946phy::TLC5946phy(SPI *spi, GPIOpin *ctrl)
{
	active = false;
	this->ctrl = ctrl;
	this->spi = spi;

	if (spi == NULL || ctrl == NULL)
	{
		iooo_debug(0, "No valid control interfaces passed to TLC5946phy\n");
		return;
	}

	blank_pin_pin = ctrl->findPinIndex((char *) "blank");
	mode_pin_pin = ctrl->findPinIndex((char *) "mode");
	xhalf_pin_pin = ctrl->findPinIndex((char *) "xhalf");
	xerr_pin_pin = ctrl->findPinIndex((char *) "xerr");

	//setup directions on control lines
	ctrl->enableOutput(blank_pin_pin, true);
	ctrl->enableOutput(mode_pin_pin, true);
	ctrl->enableOutput(xhalf_pin_pin, true);
	ctrl->enableOutput(xerr_pin_pin, false);

	active = true;
}

TLC5946phy::~TLC5946phy()
{
	iooo_debug(2, "TLC5946phy::~TLC5946phy()");
}

void TLC5946phy::setBlank(uint8_t blank)
{
	if (!active /*|| (blank_pin_addr < 0)*/)
		return;

	if (blank)
		ctrl->setBit(blank_pin_pin);
	else
		ctrl->clearBit(blank_pin_pin);
}

void TLC5946phy::setMode(uint8_t mode)
{
	if (!active || (mode_pin_pin < 0))
		return;
	iooo_debug(2, "set mode to %d\n", mode);
	if (mode)
		ctrl->setBit(mode_pin_pin);
	else
		ctrl->clearBit(mode_pin_pin);
}

void TLC5946phy::setXhalf(uint8_t xhalf)
{
	if (!active || (xhalf_pin_pin < 0))
		return;
	if (xhalf)
		ctrl->setBit(xhalf_pin_pin);
	else
		ctrl->clearBit(xhalf_pin_pin);
}

uint8_t TLC5946phy::getXerr()
{
	if (!active || (xerr_pin_pin < 0))
		return 0;
	return 0;
}

int TLC5946phy::setBitsPerWord(int bits)
{
	if (!active)
		return -ENODEV;
	return spi->setBitsPerWord(bits);
}

int TLC5946phy::setLSBFirst(bool lsb_first)
{
	if (!active)
		return -ENODEV;
	return spi->setLSBFirst(lsb_first);
}

int TLC5946phy::xfer(uint8_t buf_out[], uint8_t buf_in[], int len)
{
	if (!active)
		return -ENODEV;

	return spi->xfer1(buf_out, buf_in, len);
}
