/*
 * TestTLC5946.cpp
 *
 *  Created on: Jul 7, 2013
 *      Author: jacek
 */

#include "TestTLC5946.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

TestTLC5946::TestTLC5946(SPI* spi, char *pruBinFile)
{
	//setup control signals
	gp = GPIOoo::getInstance();

	char *tlc5946Pins[] =
		{ "P9_15", "P9_13", "P9_22", "P9_14" };
	char *tlc5946Names[] =
		{ "mode", "xerr", "blank", "gsclk" };

	blockTLC = gp->claim((char **) tlc5946Pins, 4);

	printf("Naming pins\n");
	blockTLC->namePins((char **) tlc5946Names);

	phy = new TLC5946PRUSSphy(spi, blockTLC, pruBinFile);
	phy->setMode(0);
	chain = new TLC5946chain(phy, 16);

}

TestTLC5946::~TestTLC5946()
{
	delete chain;
	delete phy;
	delete blockTLC;
}

void TestTLC5946::setLED(int led, uint16_t red, uint16_t green, uint16_t blue)
{
	//printf("led: %i, r=%i, g=%i, b=%i\n",led,red,green,blue);
	chain->setBrightness(led*3+3,blue);
	chain->setBrightness(led*3+2,green);
	chain->setBrightness(led*3+1,red);
}

void TestTLC5946::loop()
{
	float phi[15];
	float dphi = M_PI / 10000.0;
	float deltaphi = 2 * M_PI / 3;

	for (int i = 0; i < 5; i++)
	{
		phi[i * 3] = 2 * M_PI / 5 * i;
		phi[i * 3 + 1] = 2 * M_PI / 5 * i + deltaphi;
		phi[i * 3 + 2] = 2 * M_PI / 5 * i + 2 * deltaphi;
	}

	int led = 0;
	int color = 0;
	uint16_t br = (uint16_t)(0xfff * 0.008);
	 //br = (uint16_t)(0xfff );
	for (int i = 0; i < 16; i++)
	{
		chain->setBrightness(i, /*0xfff*/br);
		chain->setDOT(i, 0x3f);
	}
	chain->commit();

	sleep(1);

	color=0;
	int phase=0;
	for (;;)
	{
		/*
		 for(int i=0;i<15;i++)
		 {
		 float v=sin(phi[i])+1;
		 printf("%05.3f ",v);
		 chain.setBrightness(i,(int)(2000.0*v));
		 phi[i]+=dphi;
		 if(phi[i]>2*M_PI)
		 phi[i]=0;
		 }
		 */


		//printf("color=%i, phase=%i\n",color,phase);
		for(led=0;led<5;led++)
		{
			uint16_t r=0,g=0,b=0;
			switch(color)
			{
				case 0:	//red - traveling
					r=(led==phase) ? br : 0;
					break;
				case 1:	//green - traveling
					g=(led==phase) ? br : 0;
					break;
				case 2:	//blue - traveling
					b=(led==phase) ? br : 0;
					break;
				case 3:	//red+green - traveling
					r=g=(led==phase) ? br : 0;
					break;
				case 4:	//red+blue - traveling
					r=b=(led==phase) ? br : 0;
					break;
				case 5:	//green+blue - traveling
					g=b=(led==phase) ? br : 0;
					break;
				case 6:	//red+green+blue - all fading
					r=g=b=br >> (phase );
					break;
				default:
					color=0;
			}
			setLED(4-led,r,g,b);
		}
		chain->commit();
		//printf("\n");
		usleep(50000);

		if(++phase>=5)
		{
			phase=0;
			if(++color>6)
				color=0;
		}

#if 0
		for (int i = 0; i <= 15; i++)
		{
			switch(color)
			{
				case 0:
				case 1:
				case 2:
					chain->setBrightness(15 - i, (i == led * 3 + color) ? br : 0);
					break;
				case 3:
					chain->setBrightness(15-i,)

			}
			if (color < 3)

			else if(color<6)
			{
				chain
			}
			else
			{
				chain->setBrightness(15 - i, br >> (led * 2));
			}
		}
		led++;
		if (led > 4)
		{
			led = 0;
			color++;
			if (color > 3)
				color = 0;
		}
#endif

	}
	//phy.setMode(0);
	//usleep(500000);

}
