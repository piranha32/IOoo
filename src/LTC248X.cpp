/*
 * LTC248X.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#include "device/LTC248X.h"

#include <stdint.h>

#define LTC2485_TRY_COUNT 10
#define LTC2485_MAX_VALUE 16777216
#define LTC2485_MIN_VALUE -16777216
#define LTC2485_V_MULTIPLIER 10000000

/*
 * Family generic functions
 */

LTC248X::LTC248X(I2C *handle) :
		handle(handle), initialized(false)
{
	// Last conversion was never: epoch
	lastConv = 0;

	// Set default flags
	flags = LTC248X_DEFAULT;
}

int LTC248X::init()
{
	// Initialize with default flags or last flags used
	return init(flags);
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
		flags ^= LTC248X_REJECT_50HZ | LTC248X_REJECT_60HZ;

	if (handle->write(&flags, 1) < 0)
	{
		error("LTC248X::init() error: %s (%i)\n", strerror(errno), errno);
		return -1;
	}
	// Writes also initialize a conversion
	lastConv = clock();

	// Record current flags in use
	this->flags = flags;

	this->initialized = true;

	return 0;
}

LTC248X::~LTC248X()
{
	handle->close();
}

/*
 * Device specific functions
 */

LTC2485::LTC2485(I2C *handle) :
		LTC248X(handle)
{
}

void LTC2485::waitForConversion()
{
	// Get the correct wait time in seconds (according to datasheet)
	// These are the typical values. It can take a bit longer, but
	// this should be cancelled out by the latency of I2C.
	// If not, conversion reads are attempted multiple times anyway
	double waitTime;
	if (flags & LTC248X_REJECT_50HZ)
	{
		if (flags & LTC248X_SPEED_MODE)
			waitTime = 0.0803;
		else
			waitTime = 0.1603;
	}
	else if (flags & LTC248X_REJECT_60HZ)
	{
		if (flags & LTC248X_SPEED_MODE)
			waitTime = 0.0669;
		else
			waitTime = 0.1336;
	}
	else
	{
		// REJECT_BOTH
		if (flags & LTC248X_SPEED_MODE)
			waitTime = 0.0736;
		else
			waitTime = 0.1469;
	}

	double elapsed;
	do
	{
		elapsed = ((double) clock() - lastConv) / CLOCKS_PER_SEC;
	} while (elapsed < waitTime);
}

long LTC2485::takeMeasurement()
{
	if (!handle->isReady())
	{
		error(
				"LTC2485::takeMeasurement() error: The I2C interface is not open.\n");
		errno = EDESTADDRREQ;
		return 0;
	}

	if (!initialized) init();

	int32_t raw = 0, result = 0;

	waitForConversion();

	// Try multiple times to read the value from the ADC, just in case it's taking
	// longer than usual
	int i = LTC2485_TRY_COUNT;
	int success;
	do {
		success = handle->read(&raw, 4, false, false);
	} while (success < 0 && --i > 0);

	if (i <= 0) {
		error("LTC2485::takeMeasurement() error: %s (%d)\n",
				strerror(errno), errno);
		return 0;
	}

	// A conversion is initialized when all 32 bits are successfully read
	lastConv = clock();

	// Flip byte order
	result = raw & 0xFF;
	result = (result << 8) | ((raw >> 8) & 0xFF);
	result = (result << 8) | ((raw >> 16) & 0xFF);
	result = (result << 8) | ((raw >> 24) & 0xFF);

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

double LTC2485::takeMeasurementF()
{
	return takeMeasurement() / (LTC2485_MAX_VALUE - LTC2485_MIN_VALUE);
}

double LTC2485::takeMeasurementVolts()
{
	return ((double) takeMeasurement()) / LTC2485_V_MULTIPLIER;
}

LTC2485::~LTC2485()
{
}
