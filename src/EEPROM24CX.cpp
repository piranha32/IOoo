/*
 * EEPROM24CX.cpp
 *
 *  Created on: Nov 6, 2015
 *      Author: chris
 */

#include "../include/device/EEPROM24CX.h"

#include <unistd.h>
#include <stdint.h>
#include <netinet/in.h>

#include <vector>

/*
 * Family generic functions
 */

EEPROM24CX::EEPROM24CX(I2C *handle, size_t eepromSize, size_t pageSize,
		size_t addressLength) :
		handle(handle), eepromSize(eepromSize), pageSize(pageSize), addressLength(
				addressLength)
{
	busCache = handle->getActiveBus();
	addrCache = handle->getActiveAddress();
}

int EEPROM24CX::open()
{
	if (handle->isReady())
		return 0;

	int rc = handle->open(busCache);
	if (rc < 0)
		return rc;
	else
		return handle->setSlave(addrCache);
}

int EEPROM24CX::close()
{
	return handle->close();
}

int EEPROM24CX::read(size_t pos, size_t size, void* rbuf)
{
	if (eepromSize != EEPROM_UNKNOWN && pos >= eepromSize)
	{
		iooo_error(
				"EEPROM24CX::read() error: Tried to read past end of memory.\n");
		errno = EINVAL;
		return -1;
	}

	if (eepromSize != EEPROM_UNKNOWN && pos + size >= eepromSize)
	{
		iooo_error(
				"EEPROM24CX::read() warning: Trying to read past end of memory. Data will be truncated.\n");
		size = eepromSize - pos;
	}

	if (!waitForCompletion())
	{
		iooo_error(
				"EEPROM24CX::read() error: Timeout reached while waiting for the EEPROM to respond.\n");
		errno = ETIMEDOUT;
		return -1;
	}

	if (addressLength == EEPROM_8_ADDR)
	{
		uint8_t a = pos;
		return handle->writeRead(&a, EEPROM_8_ADDR, rbuf, size);
	}
	else if (addressLength == EEPROM_16_ADDR)
	{
		uint16_t a = htons(pos);
		return handle->writeRead(&a, EEPROM_16_ADDR, rbuf, size);
	}
	else
	{
		uint32_t a = htonl(pos);
		return handle->writeRead(&a, addressLength, rbuf, size);
	}
}

int EEPROM24CX::write(size_t pos, size_t size, const void* wbuf)
{
	if (eepromSize != EEPROM_UNKNOWN && pos >= eepromSize)
	{
		iooo_error(
				"EEPROM24CX::write() error: Tried to write past end of memory.\n");
		errno = EINVAL;
		return -1;
	}

	if (eepromSize != EEPROM_UNKNOWN && pos + size >= eepromSize)
	{
		iooo_error(
				"EEPROM24CX::write() warning: Trying to write past end of memory. Data truncated.\n");
		size = eepromSize - pos;
	}

	// Do in page-sized chunks
	int i = 0;
	int remaining = size;
	int written = 0;
	do
	{
		if (waitForCompletion())
		{
			size_t offset = i * pageSize;
			size_t addr = pos + offset;
			// Create a pointer to the data at the current page offset
			const void *data = &((const unsigned char *) wbuf)[offset];

			size_t writeSize = pageSize > remaining ? remaining : pageSize;

			iooo_debug(4, "Writing page of length %d to 0x%x\n", writeSize,
					addr);

			int w;
			if (addressLength == EEPROM_8_ADDR)
			{
				uint8_t a = addr;
				w = handle->writeWrite(&a, EEPROM_8_ADDR, data, writeSize);
			}
			else if (addressLength == EEPROM_16_ADDR)
			{
				uint16_t a = htons(addr);
				w = handle->writeWrite(&a, EEPROM_16_ADDR, data, writeSize);
			}
			else
			{
				uint32_t a = htonl(addr);
				w = handle->writeWrite(&a, addressLength, data, writeSize);
			}

			if (w < 0)
				written = w;
			else
				written += w;

			remaining -= pageSize;
		}
		else
		{
			written = -1;
		}

		i++;
	} while (remaining > 0 && written >= 0);

	return written;
}

int EEPROM24CX::erase()
{
	std::vector<unsigned char> buf(pageSize, 0xFF);
	int written = 0;

	for (int i = 0; i < eepromSize - pageSize; i += pageSize)
	{
		int rc = write(i, pageSize, &buf[0]);

		if (rc < 0)
			return rc;
		else
			written += rc;
	}

	return written;
}

size_t EEPROM24CX::getEEPROMSize()
{
	return eepromSize;
}

size_t EEPROM24CX::getPageSize()
{
	return pageSize;
}

size_t EEPROM24CX::getAddressLength()
{
	return addressLength;
}

bool EEPROM24CX::waitForCompletion()
{
	size_t addr = 0;
	char temp[1];
	bool success = false;

	double start = clock();
	double elapsed = 0;

	iooo_debug(3, "Checking if EEPROM is ready...\n");
	do
	{
		int rc = handle->writeRead(&addr, addressLength, temp, 1, false, false,
				false);
		success = rc == 1;

		if (!success)
		{
			iooo_debug(3, "Waiting...\n");
			// Upper bound on most EEPROMs is 5ms
			usleep(5000);
		}

		elapsed = ((double) clock() - start) / CLOCKS_PER_SEC;
	} while (!success && elapsed < READ_TIMEOUT_SECS);

	return success;
}

EEPROM24CX::~EEPROM24CX()
{
	handle->close();
}

EEPROM24C01::EEPROM24C01(I2C *handle) :
		EEPROM24CX(handle, EEPROM_1K, EEPROM_8_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C01::~EEPROM24C01()
{
}

EEPROM24C02::EEPROM24C02(I2C *handle) :
		EEPROM24CX(handle, EEPROM_2K, EEPROM_8_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C02::~EEPROM24C02()
{
}

EEPROM24C04::EEPROM24C04(I2C *handle) :
		EEPROM24CX(handle, EEPROM_4K, EEPROM_16_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C04::~EEPROM24C04()
{
}

EEPROM24C08::EEPROM24C08(I2C *handle) :
		EEPROM24CX(handle, EEPROM_8K, EEPROM_16_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C08::~EEPROM24C08()
{
}

EEPROM24C16::EEPROM24C16(I2C *handle) :
		EEPROM24CX(handle, EEPROM_16K, EEPROM_16_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C16::~EEPROM24C16()
{
}

EEPROM24C32::EEPROM24C32(I2C *handle) :
		EEPROM24CX(handle, EEPROM_32K, EEPROM_32_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C32::~EEPROM24C32()
{
}

EEPROM24C64::EEPROM24C64(I2C *handle) :
		EEPROM24CX(handle, EEPROM_64K, EEPROM_64_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C64::~EEPROM24C64()
{
}

EEPROM24C128::EEPROM24C128(I2C *handle) :
		EEPROM24CX(handle, EEPROM_128K, EEPROM_64_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C128::~EEPROM24C128()
{
}

EEPROM24C256::EEPROM24C256(I2C *handle) :
		EEPROM24CX(handle, EEPROM_256K, EEPROM_64_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C256::~EEPROM24C256()
{
}

EEPROM24C512::EEPROM24C512(I2C *handle) :
		EEPROM24CX(handle, EEPROM_512K, EEPROM_128_PAGE, EEPROM_16_ADDR)
{
}
EEPROM24C512::~EEPROM24C512()
{
}

EEPROM24C1024::EEPROM24C1024(I2C *handle) :
		EEPROM24CX(handle, EEPROM_1M, EEPROM_256_PAGE, EEPROM_24_ADDR)
{
}
EEPROM24C1024::~EEPROM24C1024()
{
}

EEPROM24C2048::EEPROM24C2048(I2C *handle) :
		EEPROM24CX(handle, EEPROM_2M, EEPROM_256_PAGE, EEPROM_24_ADDR)
{
}
EEPROM24C2048::~EEPROM24C2048()
{
}

EEPROM24CGeneric::EEPROM24CGeneric(I2C *handle) :
		EEPROM24CX(handle, EEPROM_UNKNOWN, EEPROM_UNKNOWN_PAGE,
		EEPROM_UNKNOWN_ADDR)
{
}
EEPROM24CGeneric::~EEPROM24CGeneric()
{
}
