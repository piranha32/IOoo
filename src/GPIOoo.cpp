/*
 * TLC5946phy.cpp
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#include "GPIOoo.h"
#include "GPIOpin.h"
#include <stdio.h>

#if defined(_HW_PLATFORM_BEAGLEBONE) 
#include "beaglebone/BeagleGoo.h"
#else
#error "No HW platform specified"
#endif

GPIOoo::~GPIOoo()
{
}

class GPIOoo* GPIOoo::getInstance()
{
#if defined(_HW_PLATFORM_BEAGLEBONE)
	static BeagleGoo instance;
#endif
	return (GPIOoo*) &instance;
}
