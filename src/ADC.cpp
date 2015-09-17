/*
 * ADC.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#if defined(_HW_PLATFORM_BEAGLEBONE)
	#include "beaglebone/BeagleADC.h"
#else
	#include "device/NativeADC.h"
#endif

NativeADC* ADC::getNativeADC(int adcNumber)
{
#if defined(_HW_PLATFORM_BEAGLEBONE)
	return new BeagleADC(adcNumber);
#else
	return new NativeADC(adcNumber);
#endif
}
