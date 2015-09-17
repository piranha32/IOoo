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
#define LTC248X_GAIN_1		0x00
#define LTC248X_GAIN_4		0x20
#define LTC248X_GAIN_8		0x40
#define LTC248X_GAIN_16		0x60
#define LTC248X_GAIN_32		0x80
#define LTC248X_GAIN_64		0xA0
#define LTC248X_GAIN_128	0xC0
#define LTC248X_GAIN_256	0xE0

#define LTC248X_TEMPERATURE	0x08
#define LTC248X_SPEED_MODE	0x01

#define LTC248X_REJECT_60HZ	0x04
#define LTC248X_REJECT_50HZ	0x02
#define LTC248X_REJECT_BOTH	0x00

class LTC248X: public ADC
{
protected:
	I2C *handle;
	int flags;

	clock_t lastConv;

	bool initialized;

	LTC248X(I2C *handle);

	/*
	 * Waits the required amount of time
	 * (chip dependent) for a reading to be
	 * taken
	 */
	virtual void waitForConversion() = 0;
public:
	/*
	 * Initializes the ADC with default settings
	 */
	virtual int init();
	/*
	 * Initializes the ADC with given settings
	 *
	 * @param flags The amount of gain, the mode of operation and
	 *				the inbuilt frequency rejection.
	 *				Gain does not apply to the LTC2485 and will be
	 *				ignored.
	 *				= LTC248X_GAIN_* | (LTC248X_TEMPERATURE)
	 *				| (LTC248X_SPEED_MODE) | LTC248X_REJECT_*
	 */
	virtual int init(int flags);

	/*
	 * Destructor for LTC248X class
	 */
	virtual ~LTC248X();
};

class LTC2485: public LTC248X
{
protected:
	virtual void waitForConversion();
public:
	/*
	 * Constructor for LTC2485 specific class
	 *
	 * @param handle Reference to I2C instance connected to
	 * 					the LTC chip. The I2C instance must
	 * 					be open and ready.
	 * @param vref The maximum input voltage of the chip
	 * @param vground The minimum input voltage of the chip
	 * 					Defaults to 0v
	 */
	LTC2485(I2C *handle);

	virtual long takeMeasurement();
	virtual double takeMeasurementF();
	virtual double takeMeasurementVolts();

	/*
	 * Destructor for the LTC2485 class
	 */
	virtual ~LTC2485();
};

#endif /* LTC248X_H_ */
