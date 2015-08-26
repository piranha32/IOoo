/*
 * BeagleADC.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef THIRD_PARTY_IOOO_SRC_DEVICE_NATIVEADC_H_
#define THIRD_PARTY_IOOO_SRC_DEVICE_NATIVEADC_H_

#include "ADC.h"
#include "debug.h"

class NativeADC : public ADC
{
protected:
	int activeADC;
	int fd;
public:
	NativeADC();
	NativeADC(int adcNumber);

	virtual int open(int adcNumber);
	virtual int close();
	virtual long takeMeasurement();
	virtual double takeMeasurementVolts();

	virtual ~NativeADC();
};

#endif /* THIRD_PARTY_IOOO_SRC_DEVICE_NATIVEADC_H_ */
