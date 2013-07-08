/*
 * HD44780phy.h
 *
 *  Created on: Jun 15, 2013
 *      Author: jacek
 */

#ifndef HD44780PHY_H_
#define HD44780PHY_H_

#include <stdint.h>

/**
 * Class defines interface to the display. Higher level functions use the interface for low level communication.
 * Implementation of the interface should support 8 or 4 bit communication (depending on hardware implementation),
 * exposing to the higher layer only 8-bit interface.
 * The implementation must honor multiple chip select (enable) lines, for displays with multiple HD44780 chips on board.
 * All requests to chips with indexes beyond the number of supported enable lines should be ignored.
 *
 * The class should not be use to drive multiple displays connected to the data and control bus, unless they are
 * supposed to act as one composite display.
 * To drive multiple independent displays sharing data and control bus each display, each display should have a separate
 * instance of HD44780phy object, controlled by separate blocks of IO lines sharing the data and control lines, with
 * different enable lines.
 */
class HD44780phy
{
	protected:
		int bits;
	public:
		/**
		 * Constants for setRS
		 */
		enum
		{
			RScommand=0,//!< RScommand - sets command mode
			RSdata=1    //!< RSdata - sets data mode
		};

		/**
		 * constants for setRW
		 */
		enum
		{
			RWwrite=0,//!< RWwrite - sets write mode
			RWread=1  //!< RWread - sets read mode
		};

		HD44780phy()
		{
			bits=0;
		}
		;
		virtual ~HD44780phy(){};

		/**
		 * @brief Method writes byte v to the display.
		 * Method writes byte v to the n-th chip on the display. It must implement all operations necessary to write the data.
		 * If the display uses 4bit interface, the method must split the value into nibbles and write them in correct order.
		 * @param n - index of the chip on the display
		 * @param x value to be written
		 *
		 */
		virtual void write(uint8_t n, uint8_t x)=0;

		/**
		 * @brief Method reports status of support for reading from the display.
		 * Method should return true if the higher level implementation can relay on reading
		 * from the display (e.g. status checking v.s. delays between operations.
		 * @return true of reading BUSY flag is supported.
		 */
		virtual bool supportsRead()=0;

		/**
		 * @brief Method reads the data from the display
		 * Method reads the data lines from the the n-th chip on the display. Register read by the method is selected
		 * by the RS line. If the hardware interface does not support reading from the display, value returned by the
		 * read function is not determined. Ability to read from the display should be tested with \a supportRead().
		 *
		 * @param n - index of the chip on the display
		 * @return value read from the display.
		 */
		virtual uint8_t read(uint8_t n)=0;

		/**
		 * @brief Method checks status of BUSY flag
		 * If the implementation does not support readback,
		 * method should always return false;
		 *
		 * DNI
		 *
		 * @param n - index of the chip on the display
		 * @return true if the display reports BUSY state. False if the display is busy performing internal
		 *         operations or readback is not supported.
		 */
		virtual bool busy(uint8_t n)=0;

		/**
		 * @brief Method reads value of the internal
		 * @return
		 */
		virtual uint8_t currentDataAddress(uint8_t n)=0;

		/**
		 * @brief Function sets selected E line to requested state.
		 * Function sets selected E line to requested state. Selection of enable line allows to support big
		 * displays with more than one controllers on board. Enable lines are numbered staring from 0.
		 * Value of \a num can not be ignored. If the hardware interface uses only one enable line,
		 * the method should respond only to num set to 0.
		 * @param num Index of E line.
		 * @param v Status of E line.
		 */
		virtual void setE(uint8_t num, uint8_t v)=0;

		/**
		 * @brief Method sets status of RS line.
		 * Method sets status of RS line selecting instruction (for write), status (for read) registers or data memory (R/W).
		 * Data:        RS = 1
		 * Instruction: RS = 0
		 * @param v requested status of RS line.
		 */
		virtual void setRS(uint8_t v)=0;

		/**
		 * @brief Method sets status of RW line.
		 * Method sets status of RW line. If the hardware interface does not support readback, this method can be empty.
		 * Read: RW=1
		 * Write: RW=0
		 * @param v requested status of RS line.
		 */
		virtual void setRW(uint8_t v)=0;

		int getBits() const
		{
			return bits;
		}
};

#endif /* HD44780PHY_H_ */
