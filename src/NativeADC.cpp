/*
 * NativeADC.cpp
 *
 *  Created on: Aug 26, 2015
 *      Author: chris
 */

#include "device/NativeADC.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 40
#define ADC_DEVICE_PATH_BASE "/sys/devices/ocp.3/helper.12/AIN"
#define ADC_MAX_VOLTAGE 1.8
#define ADC_MAX_VALUE 4096
#define ADC_CHAR_LENGTH 4

NativeADC::NativeADC()
{
	activeADC = -1;
	fd = -1;
}

NativeADC::NativeADC(int adcNumber) : NativeADC()
{
	// Beware, this only calls NativeADC::open
	// Re-implement for child classes if open differs
	open(adcNumber);
}

int NativeADC::init()
{
	// No initialization required
	return 0;
}

int NativeADC::open(int adcNumber)
{
	if (activeADC > -1) {
		close();
	}

	// Check device path
	char path[MAX_PATH_LEN];
	if (snprintf(path, MAX_PATH_LEN, "%s%d", ADC_DEVICE_PATH_BASE,
			adcNumber) >= MAX_PATH_LEN)
	{
		error("NativeADC::open() error: ADC number is too long.\n");
		errno = ENAMETOOLONG;
		return -1;
	}

	// Open device for reading only
	if ((fd = ::open(path, O_RDONLY)) < 0)
	{
		error("I2C::open() open(%s) error: %s (%d)\n", path, strerror(errno),
				errno);
		return fd; // fd is negative
	}

	activeADC = adcNumber;

	return fd;
}

int NativeADC::close()
{
	if (activeADC < 0)
		return 0;

	if ((::close(fd)) != 0)
	{
		error("NativeADC::close() close() error: %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	activeADC = -1;
	fd = -1;
	return 0;
}

long NativeADC::takeMeasurement()
{
	if (activeADC < 0) {
		error("NativeADC::takeMeasurement() error: No ADC device has been opened.\n");
		errno = EDESTADDRREQ;
		return 0;
	}

	char buf[ADC_CHAR_LENGTH];
	if (read(fd, &buf, ADC_CHAR_LENGTH) < 0) {
		error("NativeADC::takeMeasurement() read() error: %s (%d)\n", strerror(errno), errno);
		return 0;
	}

	return strtol(buf, nullptr, 10);
}

double NativeADC::takeMeasurementF()
{
	return takeMeasurement() / ADC_MAX_VALUE;
}

double NativeADC::takeMeasurementVolts()
{
	return takeMeasurementF() * ADC_MAX_VOLTAGE;
}

NativeADC::~NativeADC()
{
	close();
}
