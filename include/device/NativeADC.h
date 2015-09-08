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
private:
	int fd;
	int open();
	int close();
protected:
	char *adcPath;
	int activeADC;
public:
	NativeADC();
	NativeADC(int adcNumber);

	virtual int init(int adcNumber);

	virtual long takeMeasurement();
	virtual double takeMeasurementF();
	virtual double takeMeasurementVolts();

	virtual ~NativeADC();
};

#endif /* NATIVEADC_H_ */
