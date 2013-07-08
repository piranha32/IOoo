/*
 * GPIOpin.h
 *
 *  Created on: Jun 8, 2013
 *      Author: jacek
 */

#ifndef GPIOPIN_H_
#define GPIOPIN_H_

#include <stdlib.h>
#include <stdint.h>

class GPIO;

class GPIOpin
{
	protected:
		friend class GPIO;
		bool active;

		GPIOpin()
		{
			active = false;
		}
		;
	public:

		virtual ~GPIOpin()
		{
		}
		;

		virtual void namePin(int i, char *name)=0;
		virtual void namePins(char *names[])=0;
		virtual int findPinIndex(char *name)=0;

		/**
		 * Method enables (if enable==true) or disables (enable==false) output buffers on all lines in the block
		 * @param enable
		 */
		virtual void enableOutput(bool enable)=0;

		/**
		 * Method enables (if enable==true) or disables (enable==false) output buffers on i-th line in the block
		 * @param i
		 * @param enable
		 */
		virtual void enableOutput(int i, bool enable)=0;

		/**
		 * Method enables output buffers on lines listed in the \a outs array. Output buffers on all pins from the
		 * block not listed in the array will be disabled. Array contains indexes of the output lines, number of
		 * elements in the array is \a num.
		 * @param outs
		 * @param num
		 */
		virtual void enableOutput(int *outs, int num)=0;

		/**
		 * Method enables output buffers on lines listed in the \a ootNames array. Output buffers on all pins from the
		 * block not listed in the array will be disabled. Array contains references to strings with names of the output
		 * lines, number of elements in the array is \a num.
		 * @param outNames
		 * @param num
		 */
		virtual void enableOutput(char **outNames, int num)=0;

		/**
		 * Function writes the value to the pin. Write semantics is determined by the semantics parameter when GPIOs are claimed.
		 * @param v
		 */
		virtual void write(uint32_t v)=0;

		/**
		 * Method sets GPIO lines for which corresponding bit in the parameter \a v is set to 1. Lines whose bits are set to 0
		 * remain unchanged.
		 * @param v
		 */
		virtual void set(uint32_t v)=0;

		/**
		 * Method sets i-th bit.
		 * @param bit
		 */
		virtual void setBit(int bit)=0;

		/**
		 * Method clears GPIO lines for which corresponding bit in the parameter \a v is set to 1. Lines whose bits are set to 0
		 * remain unchanged.
		 * @param v
		 */
		virtual void clear(uint32_t v)=0;

		/**
		 * Method clears bit-th bit.
		 * @param bit
		 */
		virtual void clearBit(int bit)=0;

		/**
		 * Function returns value read from the GPIO block.
		 * @return
		 */
		virtual uint32_t read()=0;

		/**
		 * Method returns true of the block describes a valid set of GPIO lines.
		 * If method returns false, the block is useless and should not be expected to perform any operations.
		 * @return
		 */
		bool isValid()
		{
			return active;
		}
		;
};

#endif /* GPIOPIN_H_ */
