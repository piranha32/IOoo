/*
 * JDT18003T01.h
 *
 *  Created on: Jul 14, 2013
 *      Author: jacek
 */

#ifndef JDT18003T01_H_
#define JDT18003T01_H_

#include "device/ST7735phy.h"
#include <stdint.h>
#include "SPI.h"
#include "GPIOpin.h"

class JDT18003T01: public ST7735phy
{
	protected:
		SPI *spi;
		GPIOpin *ctrl;
		int reset_pin;
		int rs_pin;
		bool active;

	public:
		JDT18003T01(SPI *spi,GPIOpin *ctrl);
		virtual ~JDT18003T01();

		virtual void setReset(uint8_t reset);

		/**
		 *
		 * @param datamode
		 */
		virtual void setDataMode(bool datamode);

		/**
		 *
		 * @param wbuf
		 * @param len
		 * @return
		 */
		virtual int send(uint8_t wbuf[], int len);
};

#endif /* JDT18003T01_H_ */
