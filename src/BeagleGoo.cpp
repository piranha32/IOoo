/*
 * BeagleGoo.cpp
 *
 *  Created on: Jun 8, 2013
 *      Author: jacek
 */

#include "beaglebone/BeagleGoo.h"
#include "beaglebone/BeagleGooP.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "debug.h"

/**
 * Base addresses for GPIO blocks in memory
 */
const uint32_t BeagleGoo::gpioAddrs[] =
	{ 0x44E07000, 0x4804C000, 0x481AC000, 0x481AE000 };

struct BeagleGoo::GPIOInfo BeagleGoo::gpioInfos[] =
	{
		{ (char*) "P8_3", 1, 6, 0, 0 },
		{ (char*) "P8_4", 1, 7, 0, 0 },
		{ (char*) "P8_5", 1, 2, 0, 0 },
		{ (char*) "P8_6", 1, 3, 0, 0 },
		{ (char*) "P8_7", 2, 2, 0, 0 },
		{ (char*) "P8_8", 2, 3, 0, 0 },
		{ (char*) "P8_9", 2, 5, 0, 0 },
		{ (char*) "P8_10", 2, 4, 0, 0 },
		{ (char*) "P8_11", 1, 13, 0, 0 },
		{ (char*) "P8_12", 1, 12, 0, 0 },
		{ (char*) "P8_13", 0, 23, 0, 0 },
		{ (char*) "P8_14", 0, 26, 0, 0 },
		{ (char*) "P8_15", 1, 15, 0, 0 },
		{ (char*) "P8_16", 1, 14, 0, 0 },
		{ (char*) "P8_17", 0, 27, 0, 0 },
		{ (char*) "P8_18", 2, 1, 0, 0 },
		{ (char*) "P8_19", 0, 22, 0, 0 },
		{ (char*) "P8_20", 1, 31, 0, 0 },
		{ (char*) "P8_21", 1, 30, 0, 0 },
		{ (char*) "P8_22", 1, 5, 0, 0 },
		{ (char*) "P8_23", 1, 4, 0, 0 },
		{ (char*) "P8_24", 1, 1, 0, 0 },
		{ (char*) "P8_25", 1, 0, 0, 0 },
		{ (char*) "P8_26", 1, 29, 0, 0 },
		{ (char*) "P8_27", 2, 22, 0, 0 },
		{ (char*) "P8_28", 2, 24, 0, 0 },
		{ (char*) "P8_29", 2, 23, 0, 0 },
		{ (char*) "P8_30", 2, 25, 0, 0 },
		{ (char*) "P8_31", 0, 10, 0, 0 },
		{ (char*) "P8_32", 0, 11, 0, 0 },
		{ (char*) "P8_33", 0, 9, 0, 0 },
		{ (char*) "P8_34", 2, 17, 0, 0 },
		{ (char*) "P8_35", 0, 8, 0, 0 },
		{ (char*) "P8_36", 2, 16, 0, 0 },
		{ (char*) "P8_37", 2, 14, 0, 0 },
		{ (char*) "P8_38", 2, 15, 0, 0 },
		{ (char*) "P8_39", 2, 12, 0, 0 },
		{ (char*) "P8_40", 2, 13, 0, 0 },
		{ (char*) "P8_41", 2, 10, 0, 0 },
		{ (char*) "P8_42", 2, 11, 0, 0 },
		{ (char*) "P8_43", 2, 8, 0, 0 },
		{ (char*) "P8_44", 2, 9, 0, 0 },
		{ (char*) "P8_45", 2, 6, 0, 0 },
		{ (char*) "P8_46", 2, 7, 0, 0 },

		{ (char*) "P9_11", 0, 30, 0, 0 },
		{ (char*) "P9_12", 1, 28, 0, 0 },
		{ (char*) "P9_13", 0, 31, 0, 0 },
		{ (char*) "P9_14", 1, 18, 0, 0 },
		{ (char*) "P9_15", 1, 16, 0, 0 },
		{ (char*) "P9_16", 1, 19, 0, 0 },
		{ (char*) "P9_17", 0, 5, 0, 0 },
		{ (char*) "P9_18", 0, 4, 0, 0 },
		{ (char*) "P9_19", 0, 13, 0, 0 },
		{ (char*) "P9_20", 0, 12, 0, 0 },
		{ (char*) "P9_21", 0, 3, 0, 0 },
		{ (char*) "P9_22", 0, 2, 0, 0 },
		{ (char*) "P9_23", 1, 17, 0, 0 },
		{ (char*) "P9_24", 0, 15, 0, 0 },
		{ (char*) "P9_25", 3, 21, 0, 0 },
		{ (char*) "P9_26", 0, 14, 0, 0 },
		{ (char*) "P9_27", 3, 19, 0, 0 },
		{ (char*) "P9_28", 3, 17, 0, 0 },
		{ (char*) "P9_29", 3, 15, 0, 0 },
		{ (char*) "P9_30", 3, 21, 0, 0 },
		{ (char*) "P9_31", 3, 14, 0, 0 },
		{ (char*) "P9_41", 0, 20, 0, 0 },
		{ (char*) "P9_42", 0, 7, 0, 0 } };

size_t BeagleGoo::gpioCount = sizeof(BeagleGoo::gpioInfos) / sizeof(BeagleGoo::GPIOInfo);

uint16_t BeagleGoo::addrs[] =
	{ 0x0818, 0x081C, 0x0808,	                //p8_4, p8_5
			0x080C, 0x0890, 0x0894, 0x089C, 0x0898, //p8_6 .. p8_10
			0x0834, 0x0830, 0x0824, 0x0828, 0x083C, //p8_11 .. p8_15
			0x0838, 0x082C, 0x088C, 0x0820, 0x0884, //p8_16 .. p8_20
			0x0880, 0x0814, 0x0810, 0x0804, 0x0800, //p8_21 .. p8_25
			0x087C, 0x08E0, 0x08E8, 0x08E4, 0x08EC, //p8_26 .. p8_30
			0x08D8, 0x08DC, 0x08D4, 0x08CC, 0x08D0, //p8_31 .. p8_35
			0x08C8, 0x08C0, 0x08C4, 0x08B8, 0x08BC, //p8_36 .. p8_40
			0x08B0, 0x08B4, 0x08A8, 0x08AC, 0x08A0, //p8_41 .. p8_45
			0x08A4                                  //p8_46
		};

BeagleGoo::BeagleGoo()
{
	active = false;
	gpioFd = open("/dev/mem", O_RDWR | O_SYNC);
	if (gpioFd < 0)
	{
		iooo_debug(0, "BeagleGoo::BeagleGoo(): Can't open /dev/mem\n");
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		gpios[i] = (uint32_t *) mmap(NULL, GpioMemBlockLength,
				PROT_READ | PROT_WRITE, MAP_SHARED, gpioFd, gpioAddrs[i]);
		if (gpios[i] == MAP_FAILED )
		{
			iooo_debug(0, "GPIO Mapping failed for GPIO Module %i\n", i);
			return;
		}
		iooo_debug(1,
				"BeagleGoo::BeagleGoo() gpio[%i] at address 0x%08x mapped at 0x%08x\n",
				i, gpioAddrs[i], gpios[i]);
	}
	iooo_debug(2, "BeagleGoo successfully activated\n");
	active = true;
}

BeagleGoo::~BeagleGoo()
{
	active = false;
	for (int i = 0; i < 4; i++)
		munmap(gpios[i], GpioMemBlockLength);
	close(gpioFd);
}

GPIOpin *BeagleGoo::claim(char* names[], int num, gpioWriteSemantics semantics,
		gpioFlags flags)
{
	if (!active)
	{
		iooo_debug(1, "BeagleGoo::claim: BeagleGoo not active\n");
		return NULL;
	}

	if (num == 0)
	{
		iooo_debug(1, "BeagleGoo::claim(): num is 0\n");
		return NULL;
	}

	GPIOInfo **pininfos = new GPIOInfo*[num];
	for (int i = 0; i < num; i++)
	{
		pininfos[i] = _findGpio(names[i]);
		if (pininfos[i] == NULL)
		{
			iooo_debug(1, "Pin '%s' is not a valid GPIO pin\n", names[i]);
			delete[] pininfos;
			return NULL;
		}

		if ((pininfos[i]->refCounter > 0)
				&& ((pininfos[i]->flags & gpioExclusive)
						|| (flags & gpioExclusive)))
		{
			iooo_debug(0, "Pin '%s' already claimed and can not be shared\n",
					names[i]);
			delete[] pininfos;
			return NULL;
		}
		iooo_debug(2,
				"BeagleGoo::claim(): found pin %i: name: \"%s\", port=%i, bit=%i\n",
				i, pininfos[i]->name, pininfos[i]->gpioNum, pininfos[i]->bitNum);
	}

	iooo_debug(3, "Creating BeagleGooP\n");
	BeagleGooP *pin = new BeagleGooP(num, semantics, this);

	iooo_debug(3, "Adding pins\n");
	for (int i = 0; i < num; i++)
	{
		iooo_debug(3, "Adding pin %i\n", i);
		pin->addPin(pininfos[i]);
		pininfos[i]->refCounter++;
		if (flags & gpioExclusive)
			pininfos[i]->flags |= GPIOoo::gpioExclusive;
	}

	delete[] pininfos;
	iooo_debug(3, "BeagleGoo::claim: finish\n");
	return pin;
}

struct BeagleGoo::GPIOInfo* BeagleGoo::_findGpio(char* name)
{
	for (unsigned int i = 0; i < gpioCount; i++)
		if (strncmp(gpioInfos[i].name, name, MaxGpioNameLen) == 0)
			return (struct GPIOInfo *) &gpioInfos[i];
	iooo_debug(0, "BeagleGoo::_findGpio(): pin %s not found\n", name);
	return NULL;
}

void BeagleGoo::release(GPIOpin** gpio)
{
	if (gpio == NULL || *gpio == NULL)
		return;

	delete *gpio;
	*gpio = NULL;
}
