/*
 * TLC5946phy.h
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#ifndef TLC5946PHY_H_
#define TLC5946PHY_H_

#include "SPI.h"
#include "GPIOpin.h"

/**
 * The class acts as a layer abstracting physical interface to TLC5946.
 * Higher level class controlling behavior of the chain of the chips can
 * call functions of the interface without needing to know how they interface
 * with the actual hardware.
 * This allows to implement high-level functionality without knowing how
 * the hardware interface works.
 */
class TLC5946phy
{
	protected:
		SPI *spi;
		GPIOpin *ctrl;
		bool active;

		int blank_pin_pin;
		int mode_pin_pin;
		int xhalf_pin_pin;
		int xerr_pin_pin;

	public:
		/**
		 * Constructor initializes TLC5946 physical interface using provided
		 * SPI bus for data communication and GPIO lines as control signals.
		 * Constructor assumes that the clocking and blanking signals for the
		 * chip are generated externally and will not attempt to set them up.
		 * The "blank" line acts as "true" blank: setting the line active
		 * will blank all the outputs.
		 *
		 * Control signals should be assigned the following names:
		 *   - mode
		 *   - xhalf
		 *   - xerr
		 *   - blank
		 *
		 * GSCLK line will not be used and does not need to be allocated.
		 *
		 * @param _spi
		 * @param ctrl
		 */
		TLC5946phy(SPI *_spi, GPIOpin *ctrl);

		virtual ~TLC5946phy();

		virtual void setBlank(uint8_t blank);
		virtual void setMode(uint8_t mode);
		virtual void setXhalf(uint8_t xhalf);

		virtual uint8_t getXerr();

		//Sets the number of bits per word
		virtual int setBitsPerWord(int bits);

		//Sets
		virtual int setLSBFirst(bool lsb_first);
		virtual int xfer(uint8_t buf_out[], uint8_t buf_in[], int len);

};

#endif /* TLC5946PHY_H_ */
