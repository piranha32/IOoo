/*
 * TLC5946phy.h
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#ifndef TLC5946PRUSSPHY_H_
#define TLC5946PRUSSPHY_H_

#include "SPI.h"
#include "GPIOpin.h"
#include "TLC5946phy.h"

/**
 * The class acts as a layer abstracting physical interface to TLC5946.
 * Higher level class controlling behavior of the chain of the chips can
 * call functions of the interface without needing to know how they interface
 * with the actual hardware.
 * This allows to implement high-level functionality without knowing how
 * the hardware interface works.
 */
class TLC5946PRUSSphy:public TLC5946phy
{
	private:
		bool use_pruss;

		int gsclk_pin_pin;

	public:
		/**
		 * Constructor initializes TLC5946 physical interface using provided
		 * SPI bus for data communication and GPIO lines as control signals.
		 * Constructor initializes PRU0 unit with microcode read from the file
		 * whose name is provided as the last parameter.
		 * The microcode should generate clocking and blanking sequences required
		 * by TLC5946 chip. If the microcode can not be read from the file or
		 * PRUSS can not be initialized, the module will not be activated.
		 *
		 * Control signals should be assigned the following names:
		 *   - mode
		 *   - xhalf
		 *   - xerr
		 *   - blank
		 *   - gsclk
		 *
		 *  This function is Beaglebone-specific.
		 * @param _spi
		 * @param ctrl
		 * @param pruBinFile
		 */
		TLC5946PRUSSphy(SPI *_spi, GPIOpin *ctrl,char *pruBinFile);

		virtual ~TLC5946PRUSSphy();

		virtual void setBlank(uint8_t blank);

};

#endif /* TLC5946PRUSSPHY_H_ */
