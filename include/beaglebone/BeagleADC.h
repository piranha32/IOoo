/*
 * BeagleADC.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef THIRD_PARTY_IOOO_SRC_BEAGLEBONE_BEAGLEADC_H_
#define THIRD_PARTY_IOOO_SRC_BEAGLEBONE_BEAGLEADC_H_

#include "device/NativeADC.h"
#include "debug.h"

class BeagleADC : public NativeADC
{
public:
	BeagleADC();
	BeagleADC(int adcNumber);
	virtual ~BeagleADC();
};

#endif /* THIRD_PARTY_IOOO_SRC_BEAGLEBONE_BEAGLEADC_H_ */
