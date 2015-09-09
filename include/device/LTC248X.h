/*
 * LTC248X.h
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#ifndef LTC248X_H_
#define LTC248X_H_

#include <time.h>

#include "../ADC.h"
#include "../I2C.h"
#include "../debug.h"

#define LTC248X_DEFAULT	0x0

// Gain does NOT apply to LTC2485
// Gain is halved when speed mode is set
#define LTC248X_GAIN1	0x00
#define LTC248X_GAIN4	0x20
#define LTC248X_GAIN8	0x40
#define LTC248X_GAIN16	0x60
#define LTC248X_GAIN32	0x80
#define LTC248X_GAIN64	0xA0
#define LTC248X_GAIN128	0xC0
#define LTC248X_GAIN256	0xE0

#define LTC248X_TEMPERATURE	0x08
#define LTC248X_REJECT_60HZ	0x04
#define LTC248X_REJECT_50HZ	0x02
#define LTC248X_REJECT_BOTH	0x00
#define LTC248X_SPEED_MODE	0x01

class LTC248X: public ADC
{
protected:
	I2C *handle;
	double vref;
	int flags;

	clock_t lastConv;

	LTC248X(I2C *handle, double vref);

	virtual void waitForConversion() = 0;
public:
	virtual int init();
	virtual int init(int flags);

	virtual ~LTC248X();
};

class LTC2485: public LTC248X
{
protected:
	virtual void waitForConversion();
public:
	LTC2485(I2C *handle, double vref);

	virtual long takeMeasurement();
	virtual double takeMeasurementF();
	virtual double takeMeasurementVolts();

	virtual ~LTC2485();
};

#endif /* LTC248X_H_ */
