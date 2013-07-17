/*
 * TestTLC5946.h
 *
 *  Created on: Jul 7, 2013
 *      Author: jacek
 */

#ifndef TESTTLC5946_H_
#define TESTTLC5946_H_

#include "GPIOoo.h"
#include "GPIOpin.h"
#include "device/TLC5946PRUSSphy.h"
#include "device/TLC5946chain.h"

class TestTLC5946
{
	private:
		GPIOoo *gp;
		GPIOpin *blockTLC;
		TLC5946phy *phy;
		TLC5946chain *chain;
	public:
		TestTLC5946(SPI* spi, char *pruBinFile);
		virtual ~TestTLC5946();
		virtual void setLED(int led,uint16_t red, uint16_t gren, uint16_t blue);
		void loop();
};

#endif /* TESTTLC5946_H_ */
