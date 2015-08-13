/*
 * BeagleGoo.h
 *
 *  Created on: Jun 8, 2013
 *      Author: jacek
 */

#ifndef BEAGLEGOO_H_
#define BEAGLEGOO_H_

#include "../GPIOoo.h"
#include <stdint.h>

class BeagleGooP;

class BeagleGoo: public GPIOoo
{
	protected:
		friend class BeagleGooP;
		friend class GPIOoo;
		struct GPIOInfo
		{
				char *name;
				int gpioNum;
				int bitNum;
				//uint16_t addr;
				int refCounter;
				int flags;
		};

		bool active;
		static struct GPIOInfo gpioInfos[];
		static size_t gpioCount;
		static uint16_t addrs[];
		int gpioFd;
		uint32_t *gpios[4];
		static const uint32_t gpioAddrs[];

		struct GPIOInfo *_findGpio(char *name);

		static const int MaxGpioNameLen = 32;
		static const int GpioMemBlockLength = 0xfff;

		BeagleGoo();
	public:

		virtual ~BeagleGoo();

		virtual GPIOpin *claim(char *names[], int num,
				gpioWriteSemantics semantics, gpioFlags flags = gpioFlagsNone);
		virtual void release(GPIOpin **gpio);
};

#endif /* BEAGLEGOO_H_ */
