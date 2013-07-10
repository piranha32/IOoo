/*
 * TestGPIOButtons.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: jacek
 */

#include "TestGPIOButtons.h"
#include "stdio.h"
#include "unistd.h"

TestGPIOButtons::TestGPIOButtons()
{
	gp = GPIOoo::getInstance();
	const char *buttonPins[] =
		{ "P8_15", "P8_16", "P8_17"};
	blockButton = gp->claim((char**) buttonPins, 3);

	blockButton->enableOutput(false);
}

TestGPIOButtons::~TestGPIOButtons()
{
	delete blockButton;
}

void TestGPIOButtons::loop()
{
	for(;;)
	{
		uint32_t state=blockButton->read();
		bool buttons[3];
		uint32_t masks[3]={0x01,0x02,0x04};
		for(int i=0;i<3;i++)
			buttons[i]=(state&masks[i])==0;

		printf("Read word=0x%02x, b1=%i, b2=%i, b3=%i\n",state,buttons[0],buttons[1],buttons[2]);
		usleep(10000);
	}
}
