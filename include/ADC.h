/*
 * ADC.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef THIRD_PARTY_IOOO_SRC_ADC_H_
#define THIRD_PARTY_IOOO_SRC_ADC_H_

#include "debug.h"

class ADC
{
public:
	static ADC *getNativeADC(int adcNumber);

	virtual long takeMeasurement() = 0;
	virtual double takeMeasurementF() {
		return (double) takeMeasurement();
	}

	virtual double takeMeasurementVolts() = 0;

	virtual ~ADC() {}
};

#endif /* THIRD_PARTY_IOOO_SRC_ADC_H_ */
