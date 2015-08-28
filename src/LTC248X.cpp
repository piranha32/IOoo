/*
 * LTC248X.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#include "device/LTC248X.h"

#define ADC_MAX_VALUE 16777216
#define ADC_MIN_VALUE -16777216

LTC248X::LTC248X(I2C *handle, double vref) :
		handle(handle), vref(vref)
{
}

int LTC248X::init()
{
	return init(LTC248X_DEFAULT);
}

int LTC248X::init(int flags)
{
	if (!handle->isReady())
	{
		error("LTC248X::init() error: The I2C interface is not open.\n");
		errno = EDESTADDRREQ;
		return -1;
	}

	// If 50Hz and 60Hz to be rejected simultaneously,
	// the correct bit pattern is 0000, not 0110
	if (((flags & LTC248X_REJECT_50HZ) & LTC248X_REJECT_60HZ) != 0)
		flags ^= LTC248X_REJECT_50HZ & LTC248X_REJECT_60HZ;

	if (handle->write(&flags, 1) < 0)
	{
		error("LTC248X::init() error: %s (%i)\n", strerror(errno), errno);
		return -1;
	}

	return 0;
}

long LTC248X::takeMeasurement()
{
	if (!handle->isReady())
	{
		error(
				"LTC248X::takeMeasurement() error: The I2C interface is not open.\n");
		errno = EDESTADDRREQ;
		return 0;
	}

	int32_t result;

	if (handle->read(&result, 4) < 0)
	{
		error("LTC248X::init() error: %s (%d)\n", strerror(errno), errno);
		return 0;
	}

	// Convert the result data format to C-compatible integer
	// 31   30   ...   6   5    ...  0
	// SGN  MSB  ...  LSB  SUB  ...  SUB
	// ------------------------------------
	// MSB to LSB is 2s complement
	// Sign of 1 is positive, 0 is negative: flip bit
	result ^= 0x80000000;
	// Remove sub-LSB padding - sign bit is repeated in GCC
	// This may change for other compilers maybe?
	result >>= 6;

	return result;
}

double LTC248X::takeMeasurementF()
{
	return takeMeasurement() / (ADC_MAX_VALUE - ADC_MIN_VALUE);
}

double LTC248X::takeMeasurementVolts()
{
	return takeMeasurementF() * vref;
}

LTC248X::~LTC248X()
{
	handle->close();
}
