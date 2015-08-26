/*
 * LTC2485.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#include "device/LTC2485.h"

LTC2485::LTC2485(I2C *handle)
: handle(handle)
{
}

long LTC2485::takeMeasurement()
{
	if (!handle->isReady()) {
		error("LTC2485::takeMeasurement() error: The I2C interface is not open.\n");
		errno = EDESTADDRREQ;
		return -1;
	}

	return 0;
}

double LTC2485::takeMeasurementVolts()
{
	return takeMeasurement() * 1;//ADC_MAX_VOLTAGE / ADC_MAX_VALUE;
}

LTC2485::~LTC2485()
{
	handle->close();
}
