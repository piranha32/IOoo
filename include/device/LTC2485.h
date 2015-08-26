/*
 * LTC2485.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef THIRD_PARTY_IOOO_SRC_DEVICE_NATIVEADC_H_
#define THIRD_PARTY_IOOO_SRC_DEVICE_NATIVEADC_H_

#include "ADC.h"
#include "I2C.h"
#include "debug.h"

class LTC2485 : public ADC
{
protected:
	I2C *handle;
public:
	LTC2485(I2C *handle);

	virtual long takeMeasurement();
	virtual double takeMeasurementVolts();

	virtual ~LTC2485();
};

#endif /* THIRD_PARTY_IOOO_SRC_NATIVEADC_H_ */
