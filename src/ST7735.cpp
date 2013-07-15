/*
 * ST7735.cpp
 *
 *  Created on: Jul 14, 2013
 *      Author: jacek
 */

#include "device/ST7735.h"
#include <stdio.h>
#include <unistd.h>
#include "debug.h"


ST7735::ST7735(ST7735phy* phy)
{
	active=false;
	if(phy==NULL)
		return;
	this->phy=phy;

	active=true;
}

ST7735::~ST7735()
{
	// TODO Auto-generated destructor stub
}

void ST7735::init()
{
	reset();
	setSleep(false);
	setInverse(false);
	setIdle(false);
	on();
}

void ST7735::reset()
{
	phy->setReset(true);
	usleep(50);
	phy->setReset(false);
	usleep(10000);
}

void ST7735::setSleep(bool sleep)
{
	if(!active)
		return;
	uint8_t cmd[1];
	if(sleep)
		cmd[0]=SLPIN;
	else
		cmd[0]=SLPOUT;
	phy->setDataMode(false);
	phy->send(cmd,1);
}

void ST7735::setInverse(bool inv)
{
	if(!active)
		return;
	uint8_t cmd[1];
	if(inv)
		cmd[0]=INVON;
	else
		cmd[0]=INVOFF;
	phy->setDataMode(false);
	phy->send(cmd,1);
}

void ST7735::setGamma(int gamma)
{
	if(!active)
		return;
}

void ST7735::setIdle(bool idle)
{
	if(!active)
		return;
	uint8_t cmd[1];
	if(idle)
		cmd[0]=IDMON;
	else
		cmd[0]=IDMOFF;
	phy->setDataMode(false);
	phy->send(cmd,1);
}

void ST7735::on()
{
	if(!active)
	{
		debug(1,"ST7735 has not been activated properly!\n");
		return;
	}

	debug(1,"Turning ST7735 on\n");
	uint8_t cmd[1]={DISPON};
	phy->setDataMode(false);
	phy->send(cmd,1);
}

void ST7735::off()
{
	if(!active)
		return;
	uint8_t cmd[1]={DISPOFF};
	phy->setDataMode(false);
	phy->send(cmd,1);
}
