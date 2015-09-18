/*
 * BeagleGooP.cpp
 *
 *  Created on: Jun 9, 2013
 *      Author: jacek
 */

#include "beaglebone/BeagleGooP.h"
#include <string.h>
#include <stdio.h>
#include "debug.h"

#define DATA_OUT_REG    0x13C
#define DATA_IN_REG     0x138
#define GPIO_OE_REG     0x134
//spru73h pg 4093
#define DATA_CLEAR_REG  0x190
//spru73h pg 4094
#define DATA_SET_REG    0x194

BeagleGooP::BeagleGooP(int num, BeagleGoo::gpioWriteSemantics semantics,
		BeagleGoo *parent):GPIOpin()
{
	this->parent = parent;
	this->num = num;
	pins = new BeagleGoo::GPIOInfo *[num];
	localNames = new char *[num];
	ports = new uint32_t[num];
	masks = new uint32_t[num];
	writeSemantics = semantics;
	current = 0;

	for(int i=0;i<num;i++)
	{
		ports[i]=0;
		masks[i]=0;
		pins[i]=NULL;
	}
	iooo_debug(2,"BeagleGooP::BeagleGooP(): done\n");
}

BeagleGooP::~BeagleGooP()
{
	active = false;
	for (int i = 0; i < current; i++)
	{
		//decrease reference count on pin being released
		pins[i]->refCounter--;
		//sanity check
		if (pins[i]->refCounter < 0)
			iooo_debug(0,
					"BeagleGooP::~BeagleGooP(): reference count on pin '%s' is lower than zero\n",
					pins[i]->name);
		//clear gpioExclusive pin if present
		if (pins[i]->flags & GPIOoo::gpioExclusive)
			pins[i]->flags &= ~GPIOoo::gpioExclusive;
		//if has been allocated, delete local storage for local pin name
		if (localNames[i] != pins[i]->name)
			delete[] localNames[i];
	}

	//free memory allocated for arrays
	delete[] ports;
	delete[] masks;
	delete[] localNames;
	delete[] pins;
}

int BeagleGooP::addPin(BeagleGoo::GPIOInfo *pin)
{
	if (current >= num)
	{
		iooo_debug(2,"BeagleGooP::addPin(): current >= num\n");
		return -1;
	}

	pins[current] = pin;
	ports[current] = pin->gpioNum;
	masks[current] = 1 << (pin->bitNum);
	localNames[current] = pin->name;
	pin->refCounter++;
	iooo_debug(2,
			"BeagleGooP::addPin(): current=%i, pin->gpioNum=%i, pin->bitNum=%i, masks=%08x, localNames=\"%s\"\n",
			current, pin->gpioNum, pin->bitNum, masks[current],
			localNames[current]);
	current++;
	active=true;
	return 0;
}

void BeagleGooP::namePin(int i, char* name)
{
	if (i >= current)
		return;
	//if localNames[i]==pins[i]->name then local name is just a reference to the system pin name.
	//If so, local storage has to be allocated.
	iooo_debug(2,"Naming pin %i as \"%s\" (formerly \"%s\")\n",i,name,localNames[i]);
	if (localNames[i] == pins[i]->name)
		localNames[i] = new char[BeagleGoo::MaxGpioNameLen + 1];
	for(int j=0;j<BeagleGoo::MaxGpioNameLen + 1;j++)
		localNames[i][j]=0;
	//memset(localNames[i], 0, BeagleGoo::MaxGpioNameLen + 1);
	strncpy(localNames[i],name,BeagleGoo::MaxGpioNameLen);
}

void BeagleGooP::namePins(char *names[])
{
	for (int i = 0; i < current; i++)
		namePin(i, names[i]);
}

int BeagleGooP::findPinIndex(char* name)
{
	for (int i = 0; i < current; i++)
		if (strncmp(localNames[i], name, BeagleGoo::MaxGpioNameLen) == 0)
			return i;
	iooo_debug(0,"BeagleGooP::findPinIndex(): Index for pin %s not found\n", name);
	return -1;
}

void BeagleGooP::enableOutput(bool enable)
{
	for (int i = 0; i < current; i++)
		enableOutput(i, enable);
}

void BeagleGooP::enableOutput(int i, bool enable)
{
	iooo_debug(2,"BeagleGooP::enableOutput(): i=%i, enable=%i\n", i, enable);
	if (i < 0 || i >= current)
	{
		iooo_debug(1,"BeagleGooP::enableOutput(): Index %i out of range\n", i);
		return;
	}
	iooo_debug(2,
			"BeagleGooP::enableOutput(): enabling pin %i (%s): port=%i, mask=%08x, OE_REG=%08x\n",
			i, localNames[i], ports[i], masks[i],
			parent->gpios[ports[i]][GPIO_OE_REG / 4]);
	if (enable)
		parent->gpios[ports[i]][GPIO_OE_REG / 4] &= ~masks[i];
	else
		parent->gpios[ports[i]][GPIO_OE_REG / 4] |= masks[i];
	iooo_debug(2,"BeagleGooP::enableOutput(): port=%i, mask=%08x, OE_REG=%08x\n",
			ports[i], masks[i], parent->gpios[ports[i]][GPIO_OE_REG / 4]);
}

void BeagleGooP::enableOutput(int* outs, int num)
{
	iooo_debug(2,"BeagleGooP::enableOutput(arr): enabling %i pins\n", num);
	if (outs == NULL || num <= 0 || num>current )
	{
		iooo_debug(0,"BeagleGooP::enableOutput(): fail (current=%i, num=%i)\n",
				current, num);
		return;
	}
	bool *oe=new bool[current];
	for (int i = 0; i < current; i++)
		oe[i] = false;
	for (int i = 0; i < num; i++)
	{
		if (outs[i] < 0 || outs[i] > current)
			continue;
		oe[outs[i]] = true;
	}
	for (int i = 0; i < current; i++)
		enableOutput(i, oe[i]);
	delete[] oe;
}

void BeagleGooP::enableOutput(char** outNames, int num)
{
	iooo_debug(2,"BeagleGooP::enableOutput(names): enabling %i named pins\n", num);
	if (outNames == NULL || num <= 0 || num >= current)
	{
		iooo_debug(0,"BeagleGooP::enableOutput(): fail\n");
		return;
	}
	bool *oe=new bool[current];
	for (int i = 0; i < current; i++)
		oe[i] = false;
	for (int i = 0; i < num; i++)
	{
		if (outNames[i] == NULL)
			continue;
		int out = findPinIndex(outNames[i]);
		oe[out] = true;
	}
	for (int i = 0; i < current; i++)
		enableOutput(i, oe[i]);
	delete[] oe;
}

void BeagleGooP::write(uint32_t v)
{
	iooo_debug(3,"BeagleGooP::write(): writing %i\n",v);
	switch (writeSemantics)
	{
		case GPIOoo::gpioWrite:
		{ //Braces are necessary to limit the visibility scope of the variables.
			uint32_t portBitmasks[4] =
				{ 0, 0, 0, 0 };
			uint32_t valueBitmasks[4] =
				{ 0, 0, 0, 0 };

			uint32_t m = 1;
			for (int i = 0; i < current; i++)
			{
				portBitmasks[ports[i]] |= masks[i];
				if (v & m)
					valueBitmasks[ports[i]] |= masks[i];
				m = m << 1;
			}
			for (int i = 0; i < 4; i++)
			{
				if (portBitmasks[i] == 0)
					continue;
				uint32_t tmp = parent->gpios[i][DATA_OUT_REG / 4];
				tmp &= ~portBitmasks[i];
				tmp |= valueBitmasks[i];
				parent->gpios[i][DATA_OUT_REG / 4] = tmp;
			}
			break;
		}
		case GPIOoo::gpioWriteSetBeforeClear:
		case GPIOoo::gpioWriteClearBeforeSet:
		{
			uint32_t setBits[4] =
				{ 0, 0, 0, 0 };
			uint32_t clearBits[4] =
				{ 0, 0, 0, 0 };
			uint32_t m = 1;
			for (int i = 0; i < current; i++)
			{
				if (v & m)
					setBits[ports[i]] |= masks[i];
				else
					clearBits[ports[i]] |= masks[i];
				m = m << 1;
			}
			if (writeSemantics == GPIOoo::gpioWriteSetBeforeClear)
			{
				for (int i = 0; i < 4; i++)
				{
					parent->gpios[i][DATA_SET_REG / 4] = setBits[i];
					parent->gpios[i][DATA_CLEAR_REG / 4] = clearBits[i];
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					parent->gpios[i][DATA_CLEAR_REG / 4] = clearBits[i];
					parent->gpios[i][DATA_SET_REG / 4] = setBits[i];
				}
			}
			break;
		}

		case GPIOoo::gpioWriteAtomic: //atomic write is not supported on Beagle.
		default:
			iooo_debug(0,"BeagleGooP::write(): Incorrect semantics\n");
			//do nothing
			break;
	}
}

void BeagleGooP::setBit(int bit)
{
	if (bit < 0 || bit >= current)
		return;
	parent->gpios[ports[bit]][DATA_SET_REG/4]=masks[bit];
}

void BeagleGooP::set(uint32_t v)
{
	uint32_t setBits[4] =
		{ 0, 0, 0, 0 };
	uint32_t m = 1;
	for (int i = 0; i < current; i++)
	{
		if (v & m)
			setBits[ports[i]] |= masks[i];
		m = m << 1;
	}
	for (int i = 0; i < 4; i++)
		parent->gpios[i][DATA_SET_REG / 4] = setBits[i];

}

void BeagleGooP::clearBit(int bit)
{
	if (bit < 0 || bit >= current)
		return;
	parent->gpios[ports[bit]][DATA_CLEAR_REG/4]=masks[bit];
}

void BeagleGooP::clear(uint32_t v)
{
	uint32_t clearBits[4] =
		{ 0, 0, 0, 0 };
	uint32_t m = 1;
	for (int i = 0; i < current; i++)
	{
		if (!(v & m))
			clearBits[ports[i]] |= masks[i];
		m = m << 1;
	}
	for (int i = 0; i < 4; i++)
		parent->gpios[i][DATA_CLEAR_REG / 4] = clearBits[i];
}

uint32_t BeagleGooP::read()
{
	uint32_t tmp[4];
	for (int i = 0; i < 4; i++)
		tmp[i] = parent->gpios[i][DATA_IN_REG / 4];

	uint32_t r = 0;
	uint32_t m = 1;
	for (int i = 0; i < current; i++)
	{
		if (tmp[ports[i]] & masks[i])
			r |= m;
		m = m << 1;
	}
	return r;
}
