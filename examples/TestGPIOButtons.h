/*
 * TestGPIOButtons.h
 *
 *  Created on: Jul 10, 2013
 *      Author: jacek
 */

#ifndef TESTGPIOBUTTONS_H_
#define TESTGPIOBUTTONS_H_

#include "GPIOoo.h"
#include "GPIOpin.h"

class TestGPIOButtons
{
	protected:
		GPIOoo *gp;
		GPIOpin *blockButton;
	public:
		TestGPIOButtons();
		virtual ~TestGPIOButtons();
		virtual void loop();
};

#endif /* TESTGPIOBUTTONS_H_ */
