/*
 * BeagleADC.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#include "beaglebone/BeagleADC.h"

BeagleADC::BeagleADC()
{
}

BeagleADC::BeagleADC(int adcNumber) : NativeADC(adcNumber)
{
}

BeagleADC::~BeagleADC()
{
}
