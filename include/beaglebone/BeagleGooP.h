/*
 * BeagleGooP.h
 *
 *  Created on: Jun 9, 2013
 *      Author: jacek
 */

#ifndef BEAGLEGOOP_H_
#define BEAGLEGOOP_H_

#include "GPIOpin.h"
#include "BeagleGoo.h"
#include <stdint.h>

class BeagleGooP: public GPIOpin
{
	private:
		friend class BeagleGoo;

		BeagleGoo *parent;
		char **localNames;
		BeagleGoo::GPIOInfo **pins;
		uint32_t *ports;
		uint32_t *masks;
		int num;
		int current;
		GPIOoo::gpioWriteSemantics writeSemantics;
		int addPin(BeagleGoo::GPIOInfo *pin);
		BeagleGooP(int num, BeagleGoo::gpioWriteSemantics semantics,
				BeagleGoo *parent);
		virtual ~BeagleGooP();
	public:

		virtual void namePin(int i, char *name);
		virtual void namePins(char *names[]);
		virtual int findPinIndex(char *name);

		virtual void enableOutput(bool enable);
		virtual void enableOutput(int i, bool enable);
		virtual void enableOutput(int *outs, int num);
		virtual void enableOutput(char **outNames, int num);
		virtual void write(uint32_t v);
		virtual void set(uint32_t v);
		virtual void setBit(int bit);
		virtual void clear(uint32_t v);
		virtual void clearBit(int bit);
		virtual uint32_t read();
};

#endif /* BEAGLEGOOP_H_ */
