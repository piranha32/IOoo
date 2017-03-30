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

#ifndef ADC_DEVICE_PATH_BASE
#define ADC_DEVICE_PATH_BASE "/sys/devices/ocp.3/helper.12/AIN"
#endif
#ifndef ADC_MAX_VOLTAGE
#define ADC_MAX_VOLTAGE 1.799
#endif
#ifndef ADC_MAX_VALUE
#define ADC_MAX_VALUE 1799
#endif
#ifndef ADC_CHAR_LENGTH
#define ADC_CHAR_LENGTH 4
#endif

NativeADC::NativeADC()
{
	adcPath = new char[MAX_PATH_LEN];
	activeADC = -1;
	fd = -1;
}

NativeADC::NativeADC(int adcNumber) : NativeADC()
{
	// Beware, this only calls NativeADC::init
	// Re-implement for child classes if open differs
	init(adcNumber);
}

int NativeADC::init(int adcNumber)
{
	activeADC = adcNumber;

	// Check device path
	if (snprintf(adcPath, MAX_PATH_LEN, "%s%d", ADC_DEVICE_PATH_BASE,
			adcNumber) >= MAX_PATH_LEN)
	{
		iooo_error("NativeADC::open() error: ADC number is too long.\n");
		errno = ENAMETOOLONG;
		return -ENAMETOOLONG;
	}

	// Open device to test reading
	int success = open();
	if (success < 0) {
		activeADC = -1;
		errno = -success;
		return success;
	}
	close();

	return true;
}

int NativeADC::open()
{
	if (activeADC < 0) {
		iooo_error("NativeADC::open() error: No ADC device has been initialized.\n");
		errno = EDESTADDRREQ;
		return -EDESTADDRREQ;
	}

	if ((fd = ::open(adcPath, O_RDONLY)) < 0)
	{
		iooo_error("NativeADC::open() open(%s) error: %s (%d)\n", adcPath, strerror(errno),
				errno);
		return fd; // fd is negative
	}

	return 0;
}

int NativeADC::close()
{
	if (activeADC < 0) {
		iooo_error("NativeADC::close() error: No ADC device has been initialized.\n");
		errno = EDESTADDRREQ;
		return -EDESTADDRREQ;
	}

	if ((::close(fd)) != 0)
	{
		iooo_error("NativeADC::close() close() error: %s (%d)\n", strerror(errno), errno);
		return -errno;
	}

	return 0;
}

long NativeADC::takeMeasurement()
{
	if (activeADC < 0) {
		iooo_error("NativeADC::takeMeasurement() error: No ADC device has been initialized.\n");
		errno = EDESTADDRREQ;
		return 0;
	}

	// We must open and close on each reading (at least with the BeagleBone we do)
	if (open() < 0) return 0;

	char buf[ADC_CHAR_LENGTH];
	if (read(fd, &buf, ADC_CHAR_LENGTH) < 0) {
		iooo_error("NativeADC::takeMeasurement() read() error: %s (%d)\n", strerror(errno), errno);
		return 0;
	}

	close();

	return strtol(buf, nullptr, 10);
}

double NativeADC::takeMeasurementF()
{
	return ((double) takeMeasurement()) / ADC_MAX_VALUE;
}

double NativeADC::takeMeasurementVolts()
{
	return takeMeasurementF() * ADC_MAX_VOLTAGE;
}

NativeADC::~NativeADC()
{
	close();
}
