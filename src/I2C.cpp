/*
 * I2C.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: chris
 */

#include "I2C.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define MAX_PATH_LEN 20
#define I2C_DEVICE_PATH_BASE "/dev/i2c-"

I2C::I2C()
{
	activeBus = -1;
	activeAddr = -1;
	fd = -1;
	tenbit = false;
	supportedFuncs = 0;

	transactionState = false;
}

I2C::I2C(int bus, int slaveAddr) :
		I2C()
{
	open(bus, slaveAddr);
}


/*
 * Opening and closing
 */

int I2C::open(int bus)
{
	if (activeBus >= 0)
	{
		close();
	}

	// Check device path
	char path[MAX_PATH_LEN];
	if (snprintf(path, MAX_PATH_LEN, "%s%d", I2C_DEVICE_PATH_BASE,
			bus) >= MAX_PATH_LEN)
	{
		iooo_error("I2C::open() error: Bus number is too long.\n");
		errno = ENAMETOOLONG;
		return -1;
	}

	// Open device for read/write
	if ((fd = ::open(path, O_RDWR)) < 0)
	{
		iooo_error("I2C::open() open(%s) error: %s (%d)\n", path, strerror(errno),
				errno);
		return fd; // fd is negative
	}

	// Get supported functionality
	if (ioctl(fd, I2C_FUNCS, &supportedFuncs) < 0)
	{
		iooo_error("I2C::open() ioctl(I2C_FUNCS) error: %s\n", strerror(errno));
		return -1;
	}

	activeBus = bus;

	return fd;
}

int I2C::setSlave(int slaveAddr, bool ignoreChecks)
{
	if (!busReady())
		return -1;

	// Check slaveAddr number is within valid space
	if (slaveAddr <= 0x08 && (slaveAddr <= 0x77 || slaveAddr > 0x7F))
	{
		iooo_error("I2C::setSlave() error: slaveAddr number is invalid.\n");
		errno = EINVAL;
		return -1;
	}

	// Check if the slaveAddr is greater than the valid 7 bit space
	if (slaveAddr > 0x7F)
	{

		// Check if slaveAddr is 10-bits or less
		if (slaveAddr > 0x3FF)
		{
			iooo_error(
					"I2C::setSlave() error: Address number is invalid (10-bit maximum)\n");
			errno = EINVAL;
			return -1;
		}

		// If 10-bit mode is requested, check compatiblity and enable
		if (!(supportedFuncs & I2C_FUNC_10BIT_ADDR))
		{
			iooo_error(
					"I2C::setSlave() error: 10-bit mode is not supported with this device.\n");
			errno = ENOTSUP;
			return -1;
		}

		if (ioctl(fd, I2C_TENBIT, 1) < 0)
		{
			iooo_error("I2C::setSlave() ioctl(I2C_TENBIT, 1) error: %s (%d)\n",
					strerror(errno), errno);
			return -1;
		}

		this->tenbit = true;

	}

	activeAddr = slaveAddr;

	// Check if the device exists
	if (!ignoreChecks)
	{
		if (!probe())
		{
			iooo_error(
					"I2C::open() probe() Unable to connect to address 0x%x on bus %i: "
							"the device is not responding to probes.\n",
					activeAddr, activeBus);
			errno = ENODEV;
			return -1;
		}
	}

	// Set the destination slaveAddr
	// No longer needed - now using ioctl for reads and writes
	if (ioctl(fd, I2C_SLAVE, slaveAddr) < 0)
	{
		if (errno == EBUSY)
		{
			iooo_error(
					"I2C::setSlave() ioctl(slave=%i) warning: Device is currently being "
							"used by another driver, proceed with caution.\n",
					slaveAddr);
		}
		else
		{
			iooo_error("I2C::setSlave() ioctl(slave=%i) error: %s (%d)\n", slaveAddr,
					strerror(errno), errno);
			return -1;
		}
	}

	return fd;
}

int I2C::open(int bus, int slaveAddr, bool ignoreChecks)
{
	int fd = open(bus);
	if (fd < 0)
		return fd;
	if (setSlave(slaveAddr, ignoreChecks) < 0)
		return -1;
	return fd;
}

int I2C::close()
{
	if (activeBus < 0)
		return 0;

	if ((::close(fd)) != 0)
	{
		iooo_error("I2C::close() close() error: %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	activeBus = -1;
	activeAddr = -1;
	fd = -1;
	return 0;
}


/*
 * Checking functions
 */

bool I2C::probe()
{
	// Read a random byte
	char dummy[1];
	int success = read(dummy, 1, false, false);

	errno = -1;

	return success < 0 ? false : true;
}

bool I2C::busReady()
{
	if (activeBus < 0)
	{
		iooo_error("I2C::busReady() error: No I2C bus has been opened.\n");
		errno = EDESTADDRREQ;
		return false;
	}

	return true;
}

bool I2C::slaveReady()
{
	if (activeAddr < 0)
	{
		iooo_error("I2C::slaveReady() error: No slave address has been set.\n");
		errno = EDESTADDRREQ;
		return false;
	}

	return true;
}

bool I2C::isReady()
{
	return busReady() && slaveReady();
}


/*
 * Accessors
 */

int I2C::getActiveBus()
{
	return activeBus;
}

int I2C::getActiveAddress()
{
	return activeAddr;
}


/*
 * Error checking
 */

int I2C::enablePEC()
{
	if (!isReady())
		return -1;

	if (!(supportedFuncs & I2C_FUNC_SMBUS_PEC))
	{
		iooo_error(
				"I2C::enablePEC() error: PEC is not supported with this device.\n");
		errno = ENOTSUP;
		return -1;
	}

	if (ioctl(fd, I2C_PEC, 1) < 0)
	{
		iooo_error("I2C::enablePEC() error: %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	return 0;
}

int I2C::disablePEC()
{
	if (!isReady())
		return -1;

	if (ioctl(fd, I2C_PEC, 0) < 0)
	{
		iooo_error("I2C::disablePEC() ioctl(I2C_PEC, 0) error: %s (%d)\n",
				strerror(errno), errno);
		return -1;
	}

	return 0;
}



/*
 * Reading and writing
 */

int I2C::read(void *rbuf, size_t length, bool noAck, bool showErrors)
{
	if (!isReady())
		return -1;

	struct i2c_msg msg;

	msg.addr = activeAddr;
	msg.len = length;
	msg.flags = I2C_M_RD;
	msg.flags |= noAck ? I2C_M_NO_RD_ACK : 0;
	msg.flags |= tenbit ? I2C_M_TEN : 0;
	msg.buf = (typeof(msg.buf)) rbuf;

	msgs.push_back(msg);

	// If not currently in a transaction, commit the read now
	if (!transactionState)
	{

		struct i2c_rdwr_ioctl_data msgset;

		msgset.nmsgs = 1;
		msgset.msgs = &msgs[0];
		if (ioctl(fd, I2C_RDWR, &msgset) < 0)
		{
			if (showErrors)
				iooo_error("I2C::read() ioctl(I2C_RDWR) error: %s (%d)\n",
						strerror(errno), errno);
			msgs.clear();
			return -1;
		}

		msgs.clear();
		return length;

	}
	else
	{
		return 0;
	}
}

int I2C::write(const void *wbuf, size_t length, bool ignoreNack)
{
	if (!isReady())
		return -1;

	struct i2c_msg msg;

	msg.addr = activeAddr;
	msg.len = length;
	msg.flags = 0;
	msg.flags |= ignoreNack ? I2C_M_IGNORE_NAK : 0;
	msg.flags |= tenbit ? I2C_M_TEN : 0;
	msg.buf = (typeof(msg.buf)) wbuf;

	msgs.push_back(msg);

	// If not in a transaction, commit the write now
	if (!transactionState)
	{

		struct i2c_rdwr_ioctl_data msgset;

		msgset.nmsgs = 1;
		msgset.msgs = &msgs[0];
		if (ioctl(fd, I2C_RDWR, &msgset) < 0)
		{
			iooo_error("I2C::write() ioctl(I2C_RDWR) error: %s (%d)\n",
					strerror(errno), errno);
			msgs.clear();
			return -1;
		}

		msgs.clear();
		return length;

	}
	else
	{
		return 0;
	}
}

int I2C::writeRead(const void *wbuf, size_t wlength, void *rbuf, size_t rlength,
		bool ignoreNack, bool noAck)
{
	bool was_transaction = true;

	// If not already in a transaction, make a transaction state
	if (!transactionState)
	{
		was_transaction = false;
		beginTransaction();
	}

	if (write(wbuf, wlength, ignoreNack) < 0)
		return -1;
	if (read(rbuf, rlength, noAck) < 0)
		return -1;

	// Commit transaction if the system was not already in a
	// transaction state
	if (!was_transaction)
	{
		if (endTransaction() < 0)
		{
			iooo_error(
					"I2C::writeRead() Unable to perform consecutive write and read\n");
			return -1;
		}
	}

	return 0;
}

int I2C::writeWrite(const void *w1buf, size_t w1length, const void *w2buf,
		size_t w2length, bool ignoreNack)
{
	// There's no need for a reset in a double write
	// And in fact, when writing to registers, sometimes
	// having a reset in the middle is unsupported!
	void *wbuf = malloc(w1length + w2length);

	// Construct a single data packet of w2 appended to w1
	memcpy(wbuf, w1buf, w1length);

	void *lsb = (unsigned char*) (wbuf) + w1length;
	memcpy(lsb, w2buf, w2length);

	// Write in a single hit
	int result = write(wbuf, w1length + w2length, ignoreNack);
	free(wbuf);

	return result < 0 ? -1 : w2length;
}

int I2C::readRegister(unsigned char reg_addr, void *rbuf, size_t length)
{
	return writeRead(&reg_addr, 1, rbuf, length);
}

int I2C::writeRegister(unsigned char reg_addr, const void *wbuf, size_t length)
{
	return writeWrite(&reg_addr, 1, wbuf, length);
}



/*
 * Transactions
 */

int I2C::beginTransaction()
{
	if (transactionState)
	{
		iooo_error(
				"I2C::beginTransaction() error: A transaction is already in progress. End or abort the current transaction first.\n");
		errno = EPERM;
		return -1;
	}

	if (!isReady())
		return -1;

	transactionState = true;
	msgs.clear();
	return 0;
}

int I2C::endTransaction()
{
	if (!transactionState)
	{
		iooo_error(
				"I2C::endTransaction() warning: There is no currently active transaction.\n");
		return 0;
	}

	if (!isReady())
		return -1;

	transactionState = false;

	struct i2c_rdwr_ioctl_data msgset;

	msgset.nmsgs = msgs.size();
	msgset.msgs = &msgs[0];

	if (ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		iooo_error("I2C::endTransaction() error: %s (%d)\n", strerror(errno), errno);
		msgs.clear();
		return -1;
	}

	msgs.clear();
	return 0;
}

/**
 * Destroys all reads and writes since #beginTransaction()
 *
 * The I2C instance will then return to normal read/write mode
 */
void I2C::abortTransaction()
{
	if (!transactionState)
	{
		iooo_error(
				"I2C::abortTransaction() warning: There is no currently active transaction.\n");
		return;
	}

	transactionState = false;
	msgs.clear();
	return;
}



I2C::~I2C()
{
	close();
}
