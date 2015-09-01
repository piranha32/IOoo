/*
 * BeagleADC.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef NATIVEADC_H_
#define NATIVEADC_H_

#include "../ADC.h"
#include "../debug.h"

class NativeADC : public ADC
{
protected:
	int activeADC;
	int fd;
public:
	NativeADC();
	NativeADC(int adcNumber);

	virtual int init();

	virtual int open(int adcNumber);
	virtual int close();
	virtual long takeMeasurement();
	virtual double takeMeasurementF();
	virtual double takeMeasurementVolts();

	virtual ~NativeADC();
};

#endif /* NATIVEADC_H_ */
