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
	active_bus = -1;
	active_addr = -1;
	fd = -1;
	tenbit = false;
	supported_funcs = 0;

	transaction_state = false;
}

/*
 * Private stuff
 */

bool I2C::busReady()
{
	if (active_bus < 0)
	{
		error("I2C::busReady() error: No I2C bus has been opened.\n");
		errno = EDESTADDRREQ;
		return false;
	}

	return true;
}

bool I2C::slaveReady()
{
	if (active_addr < 0)
	{
		error("I2C::slaveReady() error: No slave address has been set.\n");
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
 * Opening and closing
 */

int I2C::open(int bus)
{
	if (active_bus >= 0)
	{
		close();
	}

	// Check device path
	char path[MAX_PATH_LEN];
	if (snprintf(path, MAX_PATH_LEN, "%s%d", I2C_DEVICE_PATH_BASE,
			bus) >= MAX_PATH_LEN)
	{
		error("I2C::open() error: Bus number is too long.\n");
		errno = ENAMETOOLONG;
		return -1;
	}

	// Open device for read/write
	if ((fd = ::open(path, O_RDWR)) < 0)
	{
		error("I2C::open() open(%s) error: %s (%d)\n", path, strerror(errno),
				errno);
		return fd; // fd is negative
	}

	// Get supported functionality
	if (ioctl(fd, I2C_FUNCS, &supported_funcs) < 0)
	{
		error("I2C::open() ioctl(I2C_FUNCS) error: %s\n", strerror(errno));
		return -1;
	}

	active_bus = bus;

	return fd;
}

int I2C::setSlave(int slave_addr, bool ignore_checks)
{
	if (!busReady())
		return -1;

	// Check slave_addr number is within valid space
	if (slave_addr <= 0x08 && (slave_addr <= 0x77 || slave_addr > 0x7F))
	{
		error("I2C::setSlave() error: slave_addr number is invalid.\n");
		errno = EINVAL;
		return -1;
	}

	// Check if the slave_addr is greater than the valid 7 bit space
	if (slave_addr > 0x7F)
	{

		// Check if slave_addr is 10-bits or less
		if (slave_addr > 0x3FF)
		{
			error(
					"I2C::setSlave() error: Address number is invalid (10-bit maximum)\n");
			errno = EINVAL;
			return -1;
		}

		// If 10-bit mode is requested, check compatiblity and enable
		if (!(supported_funcs & I2C_FUNC_10BIT_ADDR))
		{
			error(
					"I2C::setSlave() error: 10-bit mode is not supported with this device.\n");
			errno = ENOTSUP;
			return -1;
		}

		if (ioctl(fd, I2C_TENBIT, 1) < 0)
		{
			error("I2C::setSlave() ioctl(I2C_TENBIT, 1) error: %s (%d)\n",
					strerror(errno), errno);
			return -1;
		}

		this->tenbit = true;

	}

	active_addr = slave_addr;

	// Check if the device exists
	if (!ignore_checks)
	{
		if (!probe())
		{
			error("I2C::open() probe() Unable to connect to address 0x%x on bus %i: "
					"the device is not responding to probes.\n", active_addr, active_bus);
			errno = ENODEV;
			return -1;
		}
	}

	// Set the destination slave_addr
	// No longer needed - now using ioctl for reads and writes
	if (ioctl(fd, I2C_SLAVE, slave_addr) < 0)
	{
		if (errno == EBUSY)
		{
			error(
					"I2C::setSlave() ioctl(slave=%i) warning: Device is currently being "
							"used by another driver, proceed with caution.\n",
					slave_addr);
		}
		else
		{
			error("I2C::setSlave() ioctl(slave=%i) error: %s (%d)\n",
					slave_addr, strerror(errno), errno);
			return -1;
		}
	}

	return fd;
}

int I2C::open(int bus, int slave_addr, bool ignore_checks)
{
	int fd = open(bus);
	if (fd < 0)
		return fd;
	if (setSlave(slave_addr, ignore_checks) < 0)
		return -1;
	return fd;
}

int I2C::close()
{
	if (active_bus < 0)
		return 0;

	if ((::close(fd)) != 0)
	{
		error("I2C::close() close() error: %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	active_bus = -1;
	active_addr = -1;
	fd = -1;
	return 0;
}

bool I2C::probe()
{
	// Disable errors temporarily
	bool tmp = show_errors;
	show_errors = false;

	// Read a random byte
	char dummy[1];
	int success = read(dummy, 1);

	errno = -1;
	show_errors = tmp;

	return success < 0 ? false : true;
}

/*
 * Error checking
 */

int I2C::enablePEC()
{
	if (!isReady())
		return -1;

	if (!(supported_funcs & I2C_FUNC_SMBUS_PEC))
	{
		error(
				"I2C::enablePEC() error: PEC is not supported with this device.\n");
		errno = ENOTSUP;
		return -1;
	}

	if (ioctl(fd, I2C_PEC, 1) < 0)
	{
		error("I2C::enablePEC() error: %s (%d)\n", strerror(errno), errno);
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
		error("I2C::disablePEC() ioctl(I2C_PEC, 0) error: %s (%d)\n",
				strerror(errno), errno);
		return -1;
	}

	return 0;
}

/*
 * Reading and writing
 */

int I2C::read(void *rbuf, size_t length, bool no_ack)
{
	if (!isReady())
		return -1;

	struct i2c_msg msg;

	msg.addr = active_addr;
	msg.len = length;
	msg.flags = I2C_M_RD;
	msg.flags |= no_ack ? I2C_M_NO_RD_ACK : 0;
	msg.flags |= tenbit ? I2C_M_TEN : 0;
	msg.buf = (unsigned char *) rbuf;

	msgs.push_back(msg);

	// If not currently in a transaction, commit the read now
	if (!transaction_state)
	{

		struct i2c_rdwr_ioctl_data msgset;

		msgset.nmsgs = 1;
		msgset.msgs = &msgs[0];
		if (ioctl(fd, I2C_RDWR, &msgset) < 0)
		{
			error("I2C::read() ioctl(I2C_RDWR) error: %s (%d)\n",
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

int I2C::write(const void *wbuf, size_t length, bool ignore_nack)
{
	if (!isReady())
		return -1;

	struct i2c_msg msg;

	msg.addr = active_addr;
	msg.len = length;
	msg.flags = 0;
	msg.flags |= ignore_nack ? I2C_M_IGNORE_NAK : 0;
	msg.flags |= tenbit ? I2C_M_TEN : 0;
	msg.buf = (unsigned char *) wbuf;

	msgs.push_back(msg);

	// If not in a transaction, commit the write now
	if (!transaction_state)
	{

		struct i2c_rdwr_ioctl_data msgset;

		msgset.nmsgs = 1;
		msgset.msgs = &msgs[0];
		if (ioctl(fd, I2C_RDWR, &msgset) < 0)
		{
			error("I2C::write() ioctl(I2C_RDWR) error: %s (%d)\n",
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

int I2C::writeRead(const void *wbuf, size_t wlength, void *rbuf,
		size_t rlength, bool ignore_nack, bool no_ack)
{
	bool was_transaction = true;

	// If not already in a transaction, make a transaction state
	if (!transaction_state)
	{
		was_transaction = false;
		beginTransaction();
	}

	if (write(wbuf, wlength, ignore_nack) < 0) return -1;
	if (read(rbuf, rlength, no_ack) < 0) return -1;

	// Commit transaction if the system was not already in a
	// transaction state
	if (!was_transaction) {
		if (endTransaction() < 0)
		{
			error("I2C::writeRead() Unable to perform consecutive write and read\n");
			return -1;
		}
	}

	return 0;
}

int I2C::writeWrite(const void *w1buf, size_t w1length, const void *w2buf,
		size_t w2length, bool ignore_nack)
{
	// There's no need for a reset in a double write
	// And in fact, when writing to registers, sometimes
	// having a reset in the middle is unsupported!
	void *wbuf = malloc(w1length + w2length);

	// Construct a single data packet of w2 appended to w1
	memcpy(wbuf, w1buf, w1length);

	void *lsb = (unsigned char*)(wbuf) + w1length;
	memcpy(lsb, w2buf, w2length);

	// Write in a single hit
	int result = write(wbuf, w1length + w2length, ignore_nack);
	free(wbuf);

	return result < 0 ? -1 : w2length;
}

int I2C::readRegister(unsigned char reg_addr, void *rbuf, size_t length)
{
	return writeRead(&reg_addr, 1, rbuf, length);
}

int I2C::writeRegister(unsigned char reg_addr, void *wbuf, size_t length)
{
	return writeWrite(&reg_addr, 1, wbuf, length);
}

/*
 * Transactions
 */

int I2C::beginTransaction()
{
	if (transaction_state)
	{
		error(
				"I2C::beginTransaction() error: A transaction is already in progress. End or abort the current transaction first.\n");
		errno = EPERM;
		return -1;
	}

	if (!isReady())
		return -1;

	transaction_state = true;
	msgs.clear();
	return 0;
}

int I2C::endTransaction()
{
	if (!transaction_state)
	{
		error(
				"I2C::endTransaction() warning: There is no currently active transaction.\n");
		return 0;
	}

	if (!isReady())
		return -1;

	transaction_state = false;

	struct i2c_rdwr_ioctl_data msgset;

	msgset.nmsgs = msgs.size();
	msgset.msgs = &msgs[0];

	if (ioctl(fd, I2C_RDWR, &msgset) < 0)
	{
		error("I2C::endTransaction() error: %s (%d)\n", strerror(errno),
				errno);
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
	if (!transaction_state)
	{
		error(
				"I2C::abortTransaction() warning: There is no currently active transaction.\n");
		return;
	}

	transaction_state = false;
	msgs.clear();
	return;
}

I2C::~I2C()
{
	close();
}
