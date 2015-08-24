/*
 * I2C.h
 *
 *  Created on: Aug 18, 2015
 *      Author: chris
 */

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <linux/i2c.h>

#ifndef I2C_H_
#define I2C_H_

#define ENABLE_ERRORS 1

class I2C
{
private:
	int active_bus;
	int active_addr;
	int fd;
	unsigned long supported_funcs;
	bool tenbit;

	bool transaction_state;
	std::vector<struct i2c_msg> msgs;

	bool show_errors = ENABLE_ERRORS;

	/**
	 * Prints an error, maintaining the error number
	 */
	void error(const char *what, ...)
	{
		if (show_errors)
		{
			int errnobkp = errno;
			va_list args;
			va_start(args, what);
			vfprintf(stderr, what, args);
			va_end(args);
			errno = errnobkp;
		}
	}

	/**
	 * @return true if bus is set, false otherwise.
	 * 			errno is updated.
	 */
	bool bus_ready();

	/**
	 * @return true if slave address is set, false otherwise.
	 * 			errno is updated.
	 */
	bool slave_ready();

	/**
	 * @return true if bus and address are set, false otherwise.
	 * 			errno is updated.
	 */
	bool is_ready();

public:

	/**
	 * Default constructor for I2C class
	 */
	I2C();

	/**
	 * Initializes an I2C bus for reading and writing
	 *
	 * @param bus The bus of which the I2C bus is connected.
	 * 				On the BeagleBoard, this is one of
	 * 				0 = I2C0, 2 = I2C1, 1 = I2C2
	 * 				No that's not a mistake.
	 *
	 * @return Handle to I2C instance, or -1 on error.
	 * 			errno is updated.
	 */
	int open(int bus);

	/**
	 * Sets the destination address for all communications on this bus

	 * @param slave_addr Address of the device to communicate with on the bus
	 * 					Can be either a 7-bit or a 10-bit address
	 * @param ignore_checks If set to true, the slave will not be probed to
	 * 					check for an online device.
	 * 					Default is false.
	 *
	 * @return 0 on success or -1 on error.
	 * 			errno is updated.
	 */
	int set_slave(int slave_addr, bool ignore_checks = false);

	/**
	 * Opens an I2C device for reading and writing
	 *
	 * Shorthand for #open(bus) #set_slave(address)
	 *
	 * @param bus The bus of which the I2C bus is connected.
	 * 				One of: I2C0, I2C1, I2C2
	 * @param slave_addr Address of the device to communicate with on the bus
	 * 					Can be either a 7-bit or a 10-bit address
	 * @param ignore_checks If set to true, the slave will not be probed to
	 * 					check for an online device.
	 * 					Default is false.
	 *
	 * @return Handle to I2C instance, or -1 on error.
	 * 			errno is updated.
	 */
	int open(int bus, int slave_addr, bool ignore_checks = false);

	/**
	 * Closes the I2C instance
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int close();

	/**
	 * Checks the slave address for a circuit.
	 *
	 * The method reads one byte of data from the slave address.
	 *
	 * @return true if a device is detected, otherwise false
	 */
	bool probe();

	/**
	 * Enable error checking for this bus
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int enable_pec();

	/**
	 * Disable error checking for this bus
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int disable_pec();

	/**
	 * Reads n bytes from the I2C device
	 *
	 * @param rbuf[] Character buffer to read into
	 * @param length Number of bytes to read
	 * @param no_ack If set to true, no read acknowledgment is sent.
	 * 					Defaults to false.
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			errno is updated.
	 */
	int read(void *rbuf, size_t length, bool no_ack = false);

	/**
	 * Writes n bytes to the I2C device\
	 *
	 * @param wbuf[] Character buffer to write from
	 * @param length Number of bytes to write
	 * @param ignore_nack If set to true, the master will not require
	 * 						a read acknowledgment from the slave.
	 * 						Defaults to false.
	 *
	 * @return Number of bytes successfully written, or -1 for error.
	 * 			errno is updated.
	 */
	int write(const void *wbuf, size_t length, bool ignore_nack = false);

	/**
	 * Writes n bytes to the device, then reads m bytes and waits
	 * for acknowledgment.
	 *
	 * Useful for sending commands before reading data.
	 * For reading from a register, see #read_register()
	 *
	 * Convenience function for starting a transaction, making a write,
	 * then a read, then ending the transaction.
	 *
	 * There will be a reset between read and write.
	 *
	 * @param wbuf[] Character buffer to write from
	 * @param wlength Number of bytes to write
	 * @param rbuf[] Character buffer to read to
	 * @param rlength Number of bytes to read
	 * @param no_ack If set to true, no read acknowledgment is sent.
	 * 					Defaults to false.
	 * @param ignore_nack If set to true, the master will not require
	 * 						a read acknowledgment from the slave.
	 * 						Defaults to false.
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			errno is updated.
	 */
	int write_read(const void *wbuf, size_t wlength, void *rbuf, size_t rlength,
			bool no_ack = false, bool ignore_nack = false);

	/**
	 * Writes n + m bytes to the device in a single transaction
	 * and waits for acknowledgment.
	 *
	 * Useful for sending commands.
	 * For writing to a register, see #write_register()
	 *
	 * Convenience function for sending a sending two writes combined
	 * into a single write.
	 *
	 * There is no reset between the writes.
	 *
	 * @param w1buf[] Character buffer to write from in first write
	 * @param w1length Number of bytes to write in first write
	 * @param w2buf[] Character buffer to read to in second write
	 * @param w2length Number of bytes to read in second write
	 * @param ignore_nack If set to true, the master will not require
	 * 						a read acknowledgment from the slave.
	 * 						Defaults to false.
	 *
	 * @return Number of bytes successfully written in the second write,
	 * 			or -1 for error.
	 * 			errno is updated.
	 */
	int write_write(const void *w1buf, size_t w1length, const void *w2buf,
			size_t w2length, bool ignore_nack = false);

	/**
	 * Reads a value from a given register address on the device.
	 *
	 * This is shorthand for #write_read(&reg_addr, 1, rbuf, length)
	 *
	 * @param reg_addr The single-byte register address to read from
	 * 					If larger addresses need to be used, use the
	 * 					#write_read() function
	 * @param rbuf[] Character buffer to read to
	 * @param length Number of bytes to read
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			errno is updated.
	 */
	int read_register(unsigned char reg_addr, void *rbuf, size_t length);

	/**
	 * Writes a value to a given register address on the device.
	 *
	 * This is shorthand for #write_write(&reg_addr, 1, wbuf, length)
	 *
	 * @param reg_addr The single-byte register address to read from
	 * 					If larger addresses need to be used, use the
	 * 					#write_read() function
	 * @param wbuf[] Character buffer to write from
	 * @param length Number of bytes to write
	 *
	 * @return Number of bytes successfully written, or -1 for error.
	 * 			errno is updated.
	 */
	int write_register(unsigned char reg_addr, void *rbuf, size_t length);

	/**
	 * Begin a transaction for all following I2C commands
	 *
	 * Each read/write called after this function will be saved until
	 * #end_transaction() is called, where all reads and writes will be committed
	 * in order.
	 *
	 * @return 0 on success or -1 on error.
	 * 			errno is updated.
	 */
	int begin_transaction();

	/**
	 * Commits all reads and writes since #begin_transaction()
	 *
	 * The I2C instance will then return to normal read/write mode
	 *
	 * @return 0 on success or -1 on error.
	 * 			If the transaction could not be completed because the
	 * 			slave address was not set, the transaction can be tried again.
	 * 			Otherwise, the transaction will be cleared on error.
	 * 			errno is updated.
	 */
	int end_transaction();

	/**
	 * Destroys all reads and writes since #begin_transaction()
	 *
	 * The I2C instance will then return to normal read/write mode
	 */
	void abort_transaction();

	/**
	 * Destructor for I2C class
	 */
	virtual ~I2C();

};

#endif /* I2C_H_ */
