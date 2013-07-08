/*
 * SPI.cpp
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#include "SPI.h"
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
	active = false;
	mode = 0;
	bpw = 0;
	speed = 0;
	fd = -1;
	lsb_first=false;
}

int SPI::open(int bus, int channel)
{
	if (active)
		close();

	if (bus < 0 || channel < 0)
		return -ENODEV;

	char path[MAX_PATH_LEN];
	if (snprintf(path, MAX_PATH_LEN, "%s%d.%d", SPI_DEVICE_PATH_BASE, bus,
			channel) >= MAX_PATH_LEN)
		return -EINVAL;

	//printf("%s\n", path);
	if ((fd = ::open(path, O_RDWR, 0)) < 0)
	{
		printf("open(%s) failed\n",path);
		return fd;
	}

	uint8_t tmp;
	uint32_t tmp32;
	int r;
	if ((r = ioctl(fd, SPI_IOC_RD_MODE, &tmp)) < 0)
	{
		printf("ioctl(fd, SPI_IOC_RD_MODE, &tmp) failed\n");
		return r;
	}
	mode = tmp;

	if ((r = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &tmp)) < 0)
	{
		printf("ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &tmp) failed\n");
		return r;
	}
	bpw = tmp;

	if ((r = ioctl(fd, SPI_IOC_RD_LSB_FIRST, &tmp)) < 0)
	{
		printf("ioctl(fd, SPI_IOC_WR_LSB_FIRST, &tmp) failed\n");
		return r;
	}
	this->lsb_first = lsb_first;

	if ((r = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp32)) < 0)
	{
		printf("ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp) failed\n");
		return r;
	}
	speed = tmp32;

	//printf("mode=%02x, bpw=%d, speed=%d\n", mode, bpw, speed);
	active = true;
	return 1;
}

int SPI::close()
{
	if (!active)
		return -ENODEV;
	printf("SPI::close()\n");
	mode = 0;
	bpw = 0;
	speed = 0;
	active = false;
	int tmpfd = fd;
	fd = -1;
	return ::close(tmpfd);
}

int SPI::setMode(uint8_t mode)
{
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
	pol &= SPI_CPOL;
	uint8_t mode = (this->mode & ~(SPI_CPOL)) | pol;
	return setMode(mode);
}

int SPI::setClockPhase(uint8_t phase)
{
	phase &= SPI_CPHA;
	uint8_t mode = (this->mode & ~(SPI_CPHA)) | phase;
	return setMode(mode);
}

int SPI::setLSBFirst(bool lsb_first)
{
	if (!active)
		return -ENODEV;
	int r;
	//int v = (lsb_first) ? 1 : 0;
	if ((r = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb_first)) < 0)
		return r;
	this->lsb_first = lsb_first;
	return 1;
}

int SPI::setBitsPerWord(int bits)
{
	if (!active)
		return -ENODEV;
	int r;
	if ((r = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)) < 0)
		return r;
	bpw = bits;
	return 1;
}

int SPI::setSpeed(uint32_t speed)
{
	int r;
	if (!active)
		return -ENODEV;
	r = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (r < 0)
	{
		printf("ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed): %s", strerror(r));
		return r;
	}

	uint32_t tmp;
	r = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp);
	if (r < 0)
	{
		printf("ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed): %s", strerror(r));
		return r;
	}
	this->speed = tmp;
	//printf("setSpeed: requested: %dHz, set: %dHz\n",speed,this->speed);
	return 1;

}

int SPI::write(uint8_t wbuf[], int len)
{
	return ::write(fd, wbuf, len);
}

int SPI::read(uint8_t rbuf[], int len)
{
	memset(rbuf, 0, len);
	return ::read(fd, rbuf, len);
}

int SPI::xfer1(uint8_t wbuf[], uint8_t rbuf[], int len)
{
	struct spi_ioc_transfer txinfo;
	txinfo.tx_buf = (__u64 ) wbuf;
	txinfo.rx_buf = (__u64 ) rbuf;
	txinfo.len = len;
	txinfo.delay_usecs = 0;
	txinfo.speed_hz = speed;
	txinfo.bits_per_word = bpw;
	txinfo.cs_change = 1;

	//printf("fd=%d\n",fd);
	int r = ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo);
	if (r < 0)
	{
		printf("ioctl(fd, SPI_IOC_MESSAGE(1), &txinfo): %s (len=%d)\n",
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
	printf("SPI::~SPI()\n");
	close();
}

