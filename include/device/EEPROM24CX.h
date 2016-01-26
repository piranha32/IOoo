/*
 * EEPROM24CXX.h
 *
 *  Created on: Nov 6, 2015
 *      Author: chris
 */

#ifndef EEPROM24CX_H_
#define EEPROM24CX_H_

#include <time.h>

#include "../I2C.h"
#include "../debug.h"

#define READ_TIMEOUT_SECS 2

#define EEPROM_1K		128
#define EEPROM_2K		256
#define EEPROM_4K		512
#define EEPROM_8K		1024
#define EEPROM_16K		2048
#define EEPROM_32K		4096
#define EEPROM_64K		8192
#define EEPROM_128K		16384
#define EEPROM_256K		32768
#define EEPROM_512K		65536
#define EEPROM_1M		131072
#define EEPROM_2M		262144
#define EEPROM_UNKNOWN	0

#define EEPROM_8_PAGE	8
#define EEPROM_16_PAGE	16
#define EEPROM_32_PAGE	32
#define EEPROM_64_PAGE	64
#define EEPROM_128_PAGE	128
#define EEPROM_256_PAGE	256
#define EEPROM_512_PAGE	512
#define EEPROM_1K_PAGE	1024
#define EEPROM_UNKNOWN_PAGE	EEPROM_8_PAGE

#define EEPROM_8_ADDR	1
#define EEPROM_16_ADDR	2
#define EEPROM_24_ADDR	3
#define EEPROM_32_ADDR	4
#define EEPROM_UNKNOWN_ADDR	EEPROM_8_ADDR

class EEPROM24CX
{
protected:
	I2C *handle;

	const size_t eepromSize, pageSize, addressLength;

	int busCache, addrCache;

	bool waitForCompletion();
public:
	/*
	 * Constructor for any 24CX compatible EEPROM chip.
	 * It is recommended to use a chip-specific constructor such as EEPROM24C256()
	 * or if you do not know the parameters of the chip, EEPROM24CGeneric()
	 *
	 * @param handle The I2C handle. It must be initialized to the correct address first.
	 * @param eepromSize The size, in KiB, of the EEPROM chip
	 * @param pageSize The maximum number of bytes that can be written per instruction
	 * @param addressLength The number of bytes in the address
	 */
	EEPROM24CX(I2C *handle, size_t eepromSize, size_t pageSize,
			size_t addressLength);

	/**
	 * Convenience function to re-open the I2C instance of the EEPROM
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int open();
	/**
	 * Convenience function to close the I2C instance of the EEPROM
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int close();

	/**
	 * Reads n bytes from the EEPROM device
	 *
	 * @param pos Position in EEPROM to read from
	 * @param size Number of bytes to read
	 * @param rbuf[] Character buffer to read into
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			errno is updated.
	 */
	long int read(size_t pos, size_t size, void *rbuf);

	/**
	 * Writes n bytes to the EEPROM device
	 *
	 * @param pos Position in EEPROM to write to
	 * @param size Number of bytes to write
	 * @param wbuf[] Character buffer to write from
	 *
	 * @return Number of bytes successfully written, or -1 for error.
	 * 			errno is updated.
	 */
	long int write(size_t pos, size_t size, const void *wbuf);

	/**
	 * Erases the EEPROM.
	 *
	 * Will not work if the EERPOM has an undefined size
	 *
	 * @return Number of bytes successfully overwritten, or -1 for error.
	 * 			errno is updated.
	 */
	long int erase();

	/*
	 * @return Size of the EEPROM in bytes
	 */
	size_t getEEPROMSize();

	/*
	 * @return Size of each EEPROM page in bytes
	 */
	size_t getPageSize();

	/*
	 * @return Number of bytes used for addressing
	 */
	size_t getAddressLength();

	virtual ~EEPROM24CX();
};

class EEPROM24C01: public EEPROM24CX
{
public:
	EEPROM24C01(I2C *handle);
	virtual ~EEPROM24C01();
};

class EEPROM24C02: public EEPROM24CX
{
public:
	EEPROM24C02(I2C *handle);
	virtual ~EEPROM24C02();
};

class EEPROM24C04: public EEPROM24CX
{
public:
	EEPROM24C04(I2C *handle);
	virtual ~EEPROM24C04();
};

class EEPROM24C08: public EEPROM24CX
{
public:
	EEPROM24C08(I2C *handle);
	virtual ~EEPROM24C08();
};

class EEPROM24C16: public EEPROM24CX
{
public:
	EEPROM24C16(I2C *handle);
	virtual ~EEPROM24C16();
};

class EEPROM24C32: public EEPROM24CX
{
public:
	EEPROM24C32(I2C *handle);
	virtual ~EEPROM24C32();
};

class EEPROM24C64: public EEPROM24CX
{
public:
	EEPROM24C64(I2C *handle);
	virtual ~EEPROM24C64();
};

class EEPROM24C128: public EEPROM24CX
{
public:
	EEPROM24C128(I2C *handle);
	virtual ~EEPROM24C128();
};

class EEPROM24C256: public EEPROM24CX
{
public:
	EEPROM24C256(I2C *handle);
	virtual ~EEPROM24C256();
};

class EEPROM24C512: public EEPROM24CX
{
public:
	EEPROM24C512(I2C *handle);
	virtual ~EEPROM24C512();
};

class EEPROM24C1024: public EEPROM24CX
{
public:
	EEPROM24C1024(I2C *handle);
	virtual ~EEPROM24C1024();
};

class EEPROM24C2048: public EEPROM24CX
{
public:
	EEPROM24C2048(I2C *handle);
	virtual ~EEPROM24C2048();
};

class EEPROM24CGeneric: public EEPROM24CX
{
public:
	EEPROM24CGeneric(I2C *handle);
	virtual ~EEPROM24CGeneric();
};

#endif /* EEPROM24CX_H_ */
