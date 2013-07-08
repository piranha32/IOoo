/*
 * TestLCD.h
 *
 *  Created on: Jul 7, 2013
 *      Author: jacek
 */

#ifndef TESTLCD_H_
#define TESTLCD_H_

#include "GPIOoo.h"
#include "GPIOpin.h"
#include "device/HD44780gpioPhy.h"
#include "device/HD44780.h"

class TestLCD
{
	private:
		GPIOoo *gp;
		int lcdBits;
		GPIOpin *blockLCD;
		HD44780phy *lcdPhy;
		HD44780 *lcd;
	public:
		TestLCD(int bits);
		virtual ~TestLCD();
		void loop();
};

#endif /* TESTLCD_H_ */
