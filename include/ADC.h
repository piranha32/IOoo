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
	/*
	 * Retrieve an instance to the inbuilt ADC for the device if possible
	 *
	 * On a BeagleBone, this will return a BeagleADC
	 *
	 * @param adcNumber For numerically assigned ADCs, enter the ACD's id
	 * 					BeagleBones have ADCs numbered 0-7
	 * @return ADC instance
	 */
	static NativeADC *getNativeADC(int adcNumber);

	/*
	 * Takes a raw measurement directly from the ADC
	 * What this measurement represents is implementation dependent
	 * It's common to see a value in millivolts here, but to be
	 * safe, use ADC::takeMeasurementVolts to get a precise voltage
	 *
	 * Some ADCs may produce negative results
	 *
	 * @return Raw ADC measurement value
	 */
	virtual long takeMeasurement() = 0;
	/*
	 * Takes a measurement from the ADC and returns a floating
	 * point value between 0 (ground) and 1 (full-scale)
	 * or -1 (negative full-scale) and 1 (full-scale) depending
	 * on the type of ADC
	 *
	 * Useful to use for percentage
	 *
	 * @return Floating point value as a ratio of full-scale voltage
	 */
	virtual double takeMeasurementF() = 0;

	/*
	 * Takes a measurement from the ADC and returns a floating
	 * point value representing actual voltage read from the input.
	 *
	 * Scale is between the minimum and maximum ADC voltages, which
	 * may be negative.
	 *
	 * Use in favor of ADC::takeMeasurement() if you are unsure
	 * if the raw value corresponds to voltage
	 *
	 * @return Floating point value in V
	 */
	virtual double takeMeasurementVolts() = 0;

	/*
	 * Destructor for the ADC class
	 */
	virtual ~ADC() {}
};

#endif /* ADC_H_ */
