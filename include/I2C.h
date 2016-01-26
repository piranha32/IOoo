/*
 * I2C.h
 *
 *  Created on: Aug 18, 2015
 *      Author: chris
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <linux/i2c.h>

#include "debug.h"

#ifndef I2C_H_
#define I2C_H_

class I2C
{
public:
	enum byte_order {
		LSB_first, MSB_first
	};

private:
	int activeBus;
	int activeAddr;
	int fd;
	unsigned long supportedFuncs;
	bool tenbit;
	byte_order byteOrder = MSB_first;

	bool transactionState;
	std::vector<struct i2c_msg> msgs;

protected:
	/*
	 * Swaps the byte order of a character array
	 *
	 * @param target Pointer to array to be modified
	 * @param length Number of bytes in target
	 */
	void swapByteOrder(unsigned char *target, size_t length);

public:

	/**
	 * Default constructor for I2C class
	 */
	I2C();

	/**
	 * Convenience constructor for I2C class
	 *
	 * @param bus The bus of which the I2C bus is connected.
	 * 				One of: I2C0, I2C1, I2C2
	 * @param slaveAddr Address of the device to communicate with on the bus
	 * 					Can be either a 7-bit or a 10-bit address
	 */
	I2C(int bus, int slaveAddr);

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

	 * @param slaveAddr Address of the device to communicate with on the bus
	 * 					Can be either a 7-bit or a 10-bit address
	 * @param ignoreChecks If set to true, the slave will not be probed to
	 * 					check for an online device.
	 * 					Default is false.
	 *
	 * @return 0 on success or -1 on error.
	 * 			errno is updated.
	 */
	int setSlave(int slaveAddr, bool ignoreChecks = false);

	/**
	 * Opens an I2C device for reading and writing
	 *
	 * Shorthand for #open(bus) #setSlave(address)
	 *
	 * @param bus The bus of which the I2C bus is connected.
	 * 				One of: I2C0, I2C1, I2C2
	 * @param slaveAddr Address of the device to communicate with on the bus
	 * 					Can be either a 7-bit or a 10-bit address
	 * @param ignoreChecks If set to true, the slave will not be probed to
	 * 					check for an online device.
	 * 					Default is false.
	 *
	 * @return Handle to I2C file descriptor, or -1 on error.
	 * 			errno is updated.
	 */
	int open(int bus, int slaveAddr, bool ignoreChecks = false);

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
	 * @return true if bus is set, false otherwise.
	 * 			errno is updated.
	 */
	bool busReady();

	/**
	 * @return true if slave address is set, false otherwise.
	 * 			errno is updated.
	 */
	bool slaveReady();

	/**
	 * @return true if bus and address are set, false otherwise.
	 * 			errno is updated.
	 */
	bool isReady();

	/*
	 * @return Bus number in use
	 */
	int getActiveBus();

	/*
	 * @return Address in use
	 */
	int getActiveAddress();

	/**
	 * Get the order of the bytes the devices expect
	 * to be sent and received with.
	 *
	 * Default is MSB_first
	 */
	byte_order getByteOrder();

	/**
	 * Set the order of the bytes the devices expect
	 * to be sent and received with.
	 *
	 * Default is MSB_first
	 */
	void setByteOrder(byte_order order);

	/**
	 * Enable error checking for this bus
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int enablePEC();

	/**
	 * Disable error checking for this bus
	 *
	 * @return 0 on success or -1 for error.
	 * 			errno is updated.
	 */
	int disablePEC();

	/**
	 * Reads n bytes from the I2C device
	 *
	 * @param rbuf[] Character buffer to read into (big-endian).
	 * @param length Number of bytes to read.
	 * @param noAck If set to true, no read acknowledgment is sent.
	 * 					Defaults to false.
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			If called in the middle of a transaction, the function
	 * 			will return 0 on success as the bytes have not yet
	 * 			been committed.
	 * 			errno is updated.
	 */
	int read(void *rbuf, size_t length, bool noAck = false, bool showErrors =
			true);

	/**
	 * Writes n bytes to the I2C device\
	 *
	 * @param wbuf[] Character buffer to write from (big-endian).
	 * @param length Number of bytes to write.
	 * @param ignoreNack If set to true, the master will not require
	 * 						a read acknowledgment from the slave.
	 * 						Defaults to false.
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully written, or -1 for error.
	 * 			If called in the middle of a transaction, the function
	 * 			will return 0 on success as the bytes have not yet
	 * 			been committed.
	 * 			errno is updated.
	 */
	int write(const void *wbuf, size_t length, bool ignoreNack = false,
			bool showErrors = true);

	/**
	 * Writes n bytes to the device, then reads m bytes and waits
	 * for acknowledgment.
	 *
	 * Useful for sending commands before reading data.
	 * For reading from a register, see #readRegister()
	 *
	 * Convenience function for starting a transaction, making a write,
	 * then a read, then ending the transaction.
	 *
	 * There will be a reset between read and write.
	 *
	 * @param wbuf[] Character buffer to write from (big-endian).
	 * @param wlength Number of bytes to write.
	 * @param rbuf[] Character buffer to read to (big-endian).
	 * @param rlength Number of bytes to read.
	 * @param noAck If set to true, no read acknowledgment is sent.
	 * 					Defaults to false.
	 * @param ignoreNack If set to true, the master will not require
	 * 						a read acknowledgment from the slave.
	 * 						Defaults to false.
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			If called in the middle of a transaction, the function
	 * 			will return 0 on success as the bytes have not yet
	 * 			been committed.
	 * 			errno is updated.
	 */
	int writeRead(const void *wbuf, size_t wlength, void *rbuf, size_t rlength,
			bool noAck = false, bool ignoreNack = false,
			bool showErrors = true);

	/**
	 * Writes n + m bytes to the device in a single transaction
	 * and waits for acknowledgment.
	 *
	 * Useful for sending commands.
	 * For writing to a register, see #writeRegister()
	 *
	 * Convenience function for sending a sending two writes combined
	 * into a single write.
	 *
	 * There is no reset between the writes.
	 *
	 * @param w1buf[] Character buffer to write from in first write (big-endian).
	 * @param w1length Number of bytes to write in first write.
	 * @param w2buf[] Character buffer to read to in second write (big-endian).
	 * @param w2length Number of bytes to read in second write.
	 * @param ignoreNack If set to true, the master will not require
	 * 						a read acknowledgment from the slave.
	 * 						Defaults to false.
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully written in the second write,
	 * 			or -1 for error.
	 * 			If called in the middle of a transaction, the function
	 * 			will return 0 on success as the bytes have not yet
	 * 			been committed.
	 * 			errno is updated.
	 */
	int writeWrite(const void *w1buf, size_t w1length, const void *w2buf,
			size_t w2length, bool ignoreNack = false, bool showErrors = true);

	/**
	 * Reads a value from a given register address on the device.
	 *
	 * This is syntactic sugar for the #writeRead() function.
	 * For multi-byte addresses, use the aforementioned function
	 * in conjunction with htons()/htonl() for correct byte ordering.
	 *
	 * @param regAddr The register address to read from.
	 * @param rbuf[] Character buffer to read to (big-endian).
	 * @param length Number of bytes to read.
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully read, or -1 for error.
	 * 			If called in the middle of a transaction, the function
	 * 			will return 0 on success as the bytes have not yet
	 * 			been committed.
	 * 			errno is updated.
	 */
	int readRegister(unsigned char regAddr, void *rbuf, size_t length,
				bool showErrors = true);

	/**
	 * Writes a value to a given register address on the device.
	 *
	 * This is syntactic sugar for the #writeWrite() function.
	 * For multi-byte addresses, use the aforementioned function
	 * in conjunction with htons()/htonl() for correct byte ordering.
	 *
	 * @param regAddr The register address to read from.
	 * @param wbuf[] Character buffer to write from (big-endian).
	 * @param length Number of bytes to write.
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully written, or -1 for error.
	 * 			If called in the middle of a transaction, the function
	 * 			will return 0 on success as the bytes have not yet
	 * 			been committed.
	 * 			errno is updated.
	 */
	int writeRegister(unsigned char regAddr, const void *wbuf, size_t length,
			bool showErrors = true);

	/**
	 * Begin a transaction for all following I2C commands
	 *
	 * Each read/write called after this function will be saved until
	 * #end_transaction() is called, where all reads and writes will be committed
	 * in order.
	 *
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return 0 on success or -1 on error.
	 * 			errno is updated.
	 */
	int beginTransaction(bool showErrors = true);

	/**
	 * Commits all reads and writes since #beginTransaction()
	 *
	 * The I2C instance will then return to normal read/write mode
	 *
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * @return Number of bytes successfully written and read, or -1 on error.
	 * 			If the transaction could not be completed because the
	 * 			slave address was not set, the transaction can be tried again.
	 * 			Otherwise, the transaction will be cleared on error.
	 * 			errno is updated.
	 */
	int endTransaction(bool showErrors = true);

	/**
	 * Destroys all reads and writes since #beginTransaction()
	 *
	 * @param showErrors If set to false, errors will not be displayed.
	 * 					Defaults to true.
	 * 					Errors will not display if IOOO_ERROR_LEVEL is 0
	 *
	 * The I2C instance will then return to normal read/write mode
	 */
	void abortTransaction(bool showErrors = true);

	/**
	 * Destructor for I2C class
	 */
	virtual ~I2C();

};

#endif /* I2C_H_ */
