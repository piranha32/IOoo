/*
 * SPI.cpp
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#include "SPI.h"
#include "debug.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_PATH_LEN  40
#define SPI_DEVICE_PATH_BASE "/dev/spidev"

std::map<int, std::map<int, SPI::SharedResources>> SPI::share;

SPI::SPI()
{
	active_bus = -1;
	active_channel = -1;
	mode = 0;
	bpw = 0;
	speed = 0;
	fd = -1;
	lsb_first = false;
	resources = nullptr;
}

int SPI::open(int bus, int channel)
{
	// Check for valid bus and channel
	if (bus < 0 || channel < 0)
		return -ENODEV;

	// Obtain locking and chip select resources
	SharedResources *resources_tmp = &share[bus][channel];
	std::lock_guard<std::recursive_mutex> lock(resources_tmp->rwlock);

	// If a device is already open, close it before continuing further
	if (active_bus >= 0)
		close();

	iooo_debug(3, "SPI::open(): bus=%d, channel=%d\n", bus, channel);

	char path[MAX_PATH_LEN];
	if (snprintf(path, MAX_PATH_LEN, "%s%d.%d", SPI_DEVICE_PATH_BASE, bus,
			channel) >= MAX_PATH_LEN)
		return -EINVAL;

	if ((fd = ::open(path, O_RDWR, 0)) < 0)
	{
		iooo_error("open(%s) failed\n", path);
		return fd;
	}

	uint8_t tmp;
	uint32_t tmp32;
	int r;
	if ((r = ioctl(fd, SPI_IOC_RD_MODE, &tmp)) < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_RD_MODE, &tmp) failed\n");
		return r;
	}
	mode = tmp;

	if ((r = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &tmp)) < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &tmp) failed\n");
		return r;
	}
	bpw = tmp;

	if ((r = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &tmp)) < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_WR_LSB_FIRST, &tmp) failed\n");
		return r;
	}
	this->lsb_first = lsb_first;

	if ((r = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp32)) < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp) failed\n");
		return r;
	}
	speed = tmp32;

	active_bus = bus;
	active_channel = channel;
	resources = resources_tmp;
	return 1;
}

int SPI::close()
{
	// Check if chip select and mutex lock resources have been initialized
	if (resources == nullptr)
	{
		iooo_error("SPI::close(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}
	// Mutex lock on this bus and channel
	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	if (!isReady()) {
		active_bus = active_channel = -1;
		resources = nullptr;
		return -ENODEV;
	}

	iooo_debug(3, "SPI::close()\n");
	mode = 0;
	bpw = 0;
	speed = 0;
	active_bus = active_channel = -1;
	resources = nullptr;
	int tmpfd = fd;
	fd = -1;
	return ::close(tmpfd);
}

int SPI::chipSelect(GPIOpin* pin, int bit, int polarity)
{
	if (bit < 0)
		return -ENODEV;

	if (resources == nullptr)
	{
		iooo_error("SPI::chipSelect(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	// If the same chip is being selected, return
	if (resources->cspin == pin
			&& resources->csbit == bit
			&& resources->cspol == polarity)
		return 1;

	// Deselect last chip
	chipDeselect();

	// Select new chip
	resources->cspin = pin;
	resources->csbit = bit;
	resources->cspol = polarity;

	if (resources->cspol == 0)
		resources->cspin->clearBit(resources->csbit);
	else
		resources->cspin->setBit(resources->csbit);

	return 1;
}

void SPI::chipDeselect()
{
	if (resources == nullptr)
	{
		iooo_error("SPI::chipDeselect(): failed - no device has been opened\n");
		return;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	if (resources->cspin == nullptr)
		return;

	if (resources->cspol == 0)
		resources->cspin->setBit(resources->csbit);
	else
		resources->cspin->clearBit(resources->csbit);

	resources->cspin = nullptr;
	resources->csbit = -1;
	resources->cspol = -1;
}

bool SPI::busReady()
{
	return resources != nullptr && active_bus >= 0;
}

bool SPI::slaveReady()
{
	return resources != nullptr && resources->cspin != nullptr;
}

bool SPI::isReady()
{
	return resources != nullptr && active_bus >= 0 && active_channel >= 0;
}

int SPI::getActiveBus()
{
	return resources != nullptr ? active_bus : -1;
}

int SPI::getActiveChannel()
{
	return resources != nullptr ? active_channel : -1;
}

int SPI::setMode(uint8_t mode)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::setMode(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	mode &= SPI_CPHA | SPI_CPOL;
	mode = (this->mode & ~(SPI_CPHA | SPI_CPOL)) | mode;

	int r = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (r < 0)
		return r;

	r = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (r < 0)
		return r;

	this->mode = mode;

	return 1;
}

int SPI::setClockPolarity(uint8_t pol)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::setClockPolarity(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	pol &= SPI_CPOL;
	uint8_t mode = (this->mode & ~(SPI_CPOL)) | pol;
	return setMode(mode);
}

int SPI::setClockPhase(uint8_t phase)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::setClockPhase(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	phase &= SPI_CPHA;
	uint8_t mode = (this->mode & ~(SPI_CPHA)) | phase;
	return setMode(mode);
}

int SPI::setLSBFirst(bool lsb_first)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::setLSBFirst(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	if (!isReady())
		return -ENODEV;
	int r;
	if ((r = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first)) < 0)
		return r;
	this->lsb_first = lsb_first;
	return 1;
}

int SPI::setBitsPerWord(int bits)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::setBitsPerWord(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	if (!isReady())
		return -ENODEV;
	int r;
	if ((r = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)) < 0)
		return r;
	bpw = bits;
	return 1;
}

int SPI::setSpeed(uint32_t speed)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::setSpeed(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	int r;
	if (!isReady())
		return -ENODEV;
	r = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (r < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed): %s", strerror(r));
		return r;
	}

	uint32_t tmp;
	r = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp);
	if (r < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed): %s", strerror(r));
		return r;
	}
	this->speed = tmp;
	return 1;

}

int SPI::write(const void *wbuf, int len)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::write(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	return ::write(fd, wbuf, len);
}

int SPI::read(void *rbuf, int len)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::read(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	memset(rbuf, 0, len);
	return ::read(fd, rbuf, len);
}

int SPI::xfer1(const void *wbuf, void *rbuf, int len)
{
	if (resources == nullptr)
	{
		iooo_error("SPI::xfer1(): failed - no device has been opened\n");
		return -EDESTADDRREQ;
	}

	std::lock_guard<std::recursive_mutex> lock(resources->rwlock);

	struct spi_ioc_transfer txinfo;
	txinfo.tx_buf = (__u64 ) wbuf;
	txinfo.rx_buf = (__u64 ) rbuf;
	txinfo.len = len;
	txinfo.delay_usecs = 0;
	txinfo.speed_hz = speed;
	txinfo.bits_per_word = bpw;
	txinfo.cs_change = 0;

	int r = ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo);
	if (r < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo): %s (len=%d)\n",
				strerror(r), len);
		return r;
	}

	return len;
}

SPI::~SPI()
{
	iooo_debug(4, "SPI::~SPI()\n");
	close();
}

