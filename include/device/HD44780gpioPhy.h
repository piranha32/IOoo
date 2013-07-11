/*
 * HD44780gpioPhy.h
 *
 *  Created on: Jun 15, 2013
 *      Author: jacek
 */

#ifndef HD44780GPIOPHY_H_
#define HD44780GPIOPHY_H_

#include "GPIOpin.h"
#include "HD44780phy.h"

/**
 * Implementation of physical interface to text displays based on HD44780, using GPIOoo.
 * Displays with up to 8 chips are supported.
 */
class HD44780gpioPhy: public HD44780phy
{
	private:
		GPIOpin *wires;
		int d[8];
		int e[8];
		int rs;
		int rw;
		int n;
		bool writeReady;

		/**
		 * Method sets values on the lower 4 data wires
		 * @param nibble - value to set. Only lower 4 bits are important.
		 */
		void setNibble(uint8_t nibble);

		/**
		 * Method reads 4 bit value from wires d[0] .. d[3]
		 * @return value read
		 */
		uint8_t readNibble();

	public:
		/**
		 * Constructor initializes interface to text display based on HD44780. The display is controlled by
		 * named GPIO lines from the \s wires block. The class supports 4 and 8 bit interfaces. Width of the
		 * interface is determined by the number of data lines in the block. Implementation supports multiple
		 * chips on the display board. Chips are selected by lines from a block of ENABLE lines.<p>
		 *
		 * Wires in the GPIO block are identified by their names:
		 *
		 * - D[0] ... D[7] - for data wires. Width of the bus is determined by the number of data wires defined
		 * 					 in the block. If 8 wires are present, 8 bit interface will be used.
		 * 					 Otherwise if 4 or more wires are present 4-bit interface will be used.
		 * - RS - for wire controlling RS line
		 * - RW - for wire controlling R/W line
		 * - E - for enable wire if only one chip is present (alternative for E[0])
		 * - E[0] ... E[7] for Enable wires. Number of chips is determined by the finding the index of
		 * 				   first non-defined E[x] label. For a set of enable wires (E[0], E[1], E[3])
		 * 				   number of chips will be 2, because wire E[2] is not defined. If wire "E" is found,
		 * 				   wires E[x] will be ignored and only one chip will be supported.
		 *
		 * For multi-wire buses the names consist of the bus symbol (D for data, E for enable), opening square bracket,
		 * one digit of wire index and closing square bracket.
		 *
		 * @param wires - block of GPIOs intefacing the display.
		 */
		HD44780gpioPhy(GPIOpin *wires);
		virtual ~HD44780gpioPhy();

		virtual void write(uint8_t n, uint8_t x);

		virtual uint8_t read(uint8_t n);

		virtual bool busy(uint8_t n);

		virtual bool supportsRead()
		{
			return rw >= 0;
		};

		virtual uint8_t currentDataAddress(uint8_t n);

		virtual void setE(uint8_t num, uint8_t v);

		virtual void setRS(uint8_t v);

		virtual void setRW(uint8_t v);
};

#endif /* HD44780GPIOPHY_H_ */
