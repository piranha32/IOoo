/*
 * TestGPIOLeds.h
 *
 *  Created on: Jul 7, 2013
 *      Author: jacek
 */

#ifndef TESTGPIOLEDS_H_
#define TESTGPIOLEDS_H_

#include "GPIOoo.h"
#include "GPIOpin.h"

class TestGPIOLeds
{
	private:
		GPIOoo *gp;
		GPIOpin *blockLed;
	public:
		TestGPIOLeds();
		virtual ~TestGPIOLeds();
		virtual void loop()
		{
			loop(-1);
		};
		virtual void loop(int iterations);
};

#endif /* TESTGPIOLEDS_H_ */
