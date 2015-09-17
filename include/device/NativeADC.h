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
	/*
	 * Default constructor for the NativeADC class
	 */
	NativeADC();
	/*
	 * Convinience constructor for the ADC class:
	 * The ADC will be automatically initialized
	 * with the given adcNumber
	 *
	 * @param adcNumber The numeric identifier of the
	 * ADC to read from - commonly between 0 and 7
	 */
	NativeADC(int adcNumber);

	/*
	 * Initialize a specific native ADC channel
	 *
	 * @param adcNumber The numeric identifier of the
	 * ADC to read from - commonly between 0 and 7
	 */
	virtual int init(int adcNumber);

	virtual long takeMeasurement();
	virtual double takeMeasurementF();
	virtual double takeMeasurementVolts();

	/*
	 * Destructor for the NativeADC class
	 */
	virtual ~NativeADC();
};

#endif /* NATIVEADC_H_ */
