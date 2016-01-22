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

SPI::SPI()
{
	active_bus = -1;
	active_channel = -1;
	mode = 0;
	bpw = 0;
	speed = 0;
	fd = -1;
	lsb_first = false;
	cspin = nullptr;
	csbit = -1;
	cspol = -1;
}

int SPI::open(int bus, int channel)
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	if (active_bus >= 0)
		close();

	if (bus < 0 || channel < 0)
		return -ENODEV;

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
	return 1;
}

int SPI::close()
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	if (!isReady()) {
		active_bus = active_channel = -1;
		return -ENODEV;
	}

	iooo_debug(3, "SPI::close()\n");
	mode = 0;
	bpw = 0;
	speed = 0;
	active_bus = active_channel = -1;
	int tmpfd = fd;
	fd = -1;
	return ::close(tmpfd);
}

int SPI::chipSelect(GPIOpin* pin, int bit, int polarity)
{
	if (bit < 0)
		return -ENODEV;

	std::lock_guard<std::recursive_mutex> lock(rwlock);

	// If the same chip is being selected, return
	if (cspin == pin && csbit == bit && cspol == polarity)
		return 1;

	// Deselect last chip
	chipDeselect();

	// Select new chip
	cspin = pin;
	csbit = bit;
	cspol = polarity;

	if (cspol == 0)
		cspin->clearBit(csbit);
	else
		cspin->setBit(csbit);
	usleep(10000);

	return 1;
}

void SPI::chipDeselect()
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	if (cspin == nullptr)
		return;

	if (cspol == 0)
		cspin->setBit(csbit);
	else
		cspin->clearBit(csbit);
	usleep(10000);

	cspin = nullptr;
	csbit = -1;
	cspol = -1;
}

bool SPI::busReady()
{
	return active_bus >= 0;
}

bool SPI::slaveReady()
{
	return cspin != nullptr;
}

bool SPI::isReady()
{
	return active_bus >= 0 && active_channel >= 0;
}

int SPI::getActiveBus()
{
	return active_bus;
}

int SPI::getActiveChannel()
{
	return active_channel;
}

int SPI::setMode(uint8_t mode)
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

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
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	pol &= SPI_CPOL;
	uint8_t mode = (this->mode & ~(SPI_CPOL)) | pol;
	return setMode(mode);
}

int SPI::setClockPhase(uint8_t phase)
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	phase &= SPI_CPHA;
	uint8_t mode = (this->mode & ~(SPI_CPHA)) | phase;
	return setMode(mode);
}

int SPI::setLSBFirst(bool lsb_first)
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

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
	std::lock_guard<std::recursive_mutex> lock(rwlock);

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
	std::lock_guard<std::recursive_mutex> lock(rwlock);

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
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	return ::write(fd, wbuf, len);
}

int SPI::read(void *rbuf, int len)
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	memset(rbuf, 0, len);
	return ::read(fd, rbuf, len);
}

int SPI::xfer1(const void *wbuf, void *rbuf, int len)
{
	std::lock_guard<std::recursive_mutex> lock(rwlock);

	struct spi_ioc_transfer txinfo;
	txinfo.tx_buf = (__u64 ) wbuf;
	txinfo.rx_buf = (__u64 ) rbuf;
	txinfo.len = len;
	txinfo.delay_usecs = 0;
	txinfo.speed_hz = speed;
	txinfo.bits_per_word = bpw;
	txinfo.cs_change = 1;

	int r = ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo);
	if (r < 0)
	{
		iooo_error("ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo): %s (len=%d)\n",
				strerror(r), len);
		return r;
	}

	//deactivate CS line
	//uint8_t tmp;
	//::read(fd, &tmp, 0);
	return len;
}

SPI::~SPI()
{
	iooo_debug(4, "SPI::~SPI()\n");
	close();
}

