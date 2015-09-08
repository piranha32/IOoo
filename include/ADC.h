/*
 * ADC.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef ADC_H_
#define ADC_H_

#include "debug.h"

// Let the compiler know that this class exists somewhere in the chain
class NativeADC;

class ADC
{
public:
	static NativeADC *getNativeADC(int adcNumber);

	virtual long takeMeasurement() = 0;
	virtual double takeMeasurementF() = 0;

	virtual double takeMeasurementVolts() = 0;

	virtual ~ADC() {}
};

#endif /* ADC_H_ */
