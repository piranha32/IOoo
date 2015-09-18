/*
 * HD44780gpioPhy.cpp
 *
 *  Created on: Jun 15, 2013
 *      Author: jacek
 */

#include "device/HD44780gpioPhy.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "debug.h"

#define BUSY_BIT 0x80
#define ADDRESS_BITS 0x7f

HD44780gpioPhy::HD44780gpioPhy(GPIOpin *wires) :
		HD44780phy()
{
	this->wires = wires;

	for (int i = 0; i < 8; i++)
	{
		d[i] = -1;
		e[i] = -1;
	}
	rs = -1;
	rw = -1;
	n = 0;
	writeReady = false;

	rs = wires->findPinIndex((char *) "RS");
	rw = wires->findPinIndex((char *) "RW");
	e[0] = wires->findPinIndex((char *) "E");
	n = 1;

	char buf[10];
	if (e[0] < 0)
	{
		for (n = 0; n < 8; n++)
		{
			sprintf(buf, "E[%i]", n);
			e[n] = wires->findPinIndex(buf);
			if (e[n] < 0)
				break;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		sprintf(buf, "D[%i]", i);
		d[i] = wires->findPinIndex(buf);
		iooo_debug(2,
				"HD44780gpioPhy::HD44780gpioPhy(): Index for pin d[%i] is %i\n",
				i, d[i]);
		if (d[i] < 0)
		{
			iooo_debug(2,
					"HD44780gpioPhy::HD44780gpioPhy() Data line search interrupted at i=%i\n",
					i);
			break;
		}
		if (i == 7)
			bits = 8;
		else if (i >= 3)
			bits = 4;
	}
	iooo_debug(2, "HD44780gpioPhy::HD44780gpioPhy(): detected %i bit interface\n",
			bits);
	if (bits == 0)
		return;
	wires->enableOutput(true);
	writeReady = true;
}

HD44780gpioPhy::~HD44780gpioPhy()
{
	bits = 0;
}

void HD44780gpioPhy::setNibble(uint8_t nibble)
{
	uint8_t m = 1;
	for (int i = 0; i < 4; i++)
	{
		if (nibble & m)
			wires->setBit(d[i]);
		else
			wires->clearBit(d[i]);
		m = m << 1;
	}
}

void HD44780gpioPhy::write(uint8_t n, uint8_t x)
{
	if (bits < 4)
		return;

	// if output buffers are disabled, enable them
	if (!writeReady)
	{
		wires->enableOutput(d, bits);
		writeReady = true;
	}

	//set write mode
	setRW(0);
	if (bits == 4)
	{
		iooo_debug(3, "Writing 0x%02x in 4 bit mode\n", x);
		//write higher nibble
		setNibble((x >> 4) & 0x0f);
		setE(n, 1);
		usleep(100);
		setE(n, 0);

		usleep(200);
		//write lower nibble
		setNibble(x & 0x0f);
		setE(n, 1);
		usleep(100);
		setE(n, 0);
	}
	else
	{
		iooo_debug(3, "Writing 0x%02x in 8 bit mode\n", x);
		uint8_t mask = 1;
		for (int i = 0; i < 8; i++)
		{
			if (x & mask)
				wires->setBit(d[i]);
			else
				wires->clearBit(d[i]);
			mask = mask << 1;
		}
		setE(n, 1);
		usleep(5);
		setE(n, 0);
	}
}

uint8_t HD44780gpioPhy::readNibble()
{
	uint8_t r = 0;
	uint32_t v = wires->read();
	uint8_t mask = 1;
	for (int i = 0; i < 4; i++)
	{
		if (v & (1 << d[i]))
			r |= mask;
		mask = mask << 1;
	}

	return r;
}

uint8_t HD44780gpioPhy::read(uint8_t n)
{
	if (!supportsRead() || bits < 4)
		return 0;

	uint8_t r = 0;

	// if output buffers are enabled, disable them
	if (writeReady)
	{
		for (int i = 0; i < bits; i++)
			wires->enableOutput(d[i], false);
		writeReady = false;
	}

	setRW(1);
	if (bits == 4)
	{
		setE(n, 1);
		usleep(10);
		uint8_t nib = readNibble() & 0x0f;
		setE(n, 0);
		r = nib << 4;
		usleep(10000);
		setE(n, 1);
		usleep(10);
		nib = readNibble() & 0x0f;
		setE(n, 0);
		r |= nib;
	}
	else
	{
		setE(n, 1);
		usleep(100);
		uint32_t v = wires->read();
		setE(n, 0);
		uint8_t mask = 1;

		for (int i = 0; i < 8; i++)
		{
			if (v & (1 << d[i]))
				r |= mask;
			mask = mask << 1;
		}
	}
	return r;
}

bool HD44780gpioPhy::busy(uint8_t n)
{
	return (read(n) & BUSY_BIT) != 0;
}

uint8_t HD44780gpioPhy::currentDataAddress(uint8_t n)
{
	return (read(n) & ADDRESS_BITS);
}

void HD44780gpioPhy::setE(uint8_t num, uint8_t v)
{
	if (num < 0 || num >= n)
	{
		iooo_debug(0, "HD44780gpioPhy::setE(%i,%i): fail: num=%i, n=%i\n", num, v,
				num, n);
		return;
	}
	if (v)
		wires->setBit(e[num]);
	else
		wires->clearBit(e[num]);
}

void HD44780gpioPhy::setRS(uint8_t v)
{
	if (v)
		wires->setBit(rs);
	else
		wires->clearBit(rs);
}

void HD44780gpioPhy::setRW(uint8_t v)
{
	if (rw < 0)
		return;
	if (v)
		wires->setBit(rw);
	else
		wires->clearBit(rw);
}
