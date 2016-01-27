/*
 * SPI.h
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#include <stdint.h>
#include <linux/spi/spidev.h>

#include <mutex>
#include <map>

#include "GPIOpin.h"

#ifndef SPI_H_
#define SPI_H_

class SPI
{
	private:
		uint8_t mode;
		uint8_t bpw;
		bool lsb_first;
		uint32_t speed;
		int active_bus;
		int active_channel;
		int fd;

		struct SharedResources {
			GPIOpin *volatile cspin = nullptr;
			volatile int csbit = -1;
			volatile int cspol = -1;
			std::recursive_mutex rwlock;
		};

		static std::map<int, std::map<int, SharedResources>> share;
		SharedResources *resources;
	public:

		/**
		 * Default constructor for SPI class.
		 */
		SPI();

		/**
		 * Function opens the SPI device.
		 * @param bus
		 * @param channel
		 * @return
		 */
		int open(int bus, int channel);

		/**
		 * Function closes the SPI device.
		 * @return
		 */
		int close();

		/**
		 * Function selects a chip on the SPI bus.
		 * Will wait for current operation to finish.
		 * @param pin GPIO control pin for CS line of chip
		 * @param bit The control bit within the control pin (defaults to bit 0)
		 * @param polarity Activation logic. Set to 0 or 1 (defaults to 0, ~CS)
		 * @return negative on failure. ENODEV => invalid bit number
		 */
		int chipSelect(GPIOpin *pin, int bit = 0, int polarity = 0);

		/**
		 * Function deselects currently selected chip on SPI bus
		 * Will wait for current operation to finish.
		 */
		void chipDeselect();

		/**
		 * @return true if bus is set, false otherwise.
		 * 			errno is updated.
		 */
		bool busReady();

		/**
		 * @return true if a chip is selected, false otherwise.
		 * 			errno is updated.
		 */
		bool slaveReady();

		/**
		 * @return true if bus and channel are set, false otherwise.
		 * 			errno is updated.
		 */
		bool isReady();

		/*
		 * @return Bus number in use
		 */
		int getActiveBus();

		/*
		 * @return Bus channel in use
		 */
		int getActiveChannel();

		/**
		 * Functions sets the idle level and active edge of the clock signal.
		 * @param mode
		 * @return
		 */
		int setMode(uint8_t mode);

		/**
		 * Function sets idle level for the clock signal.
		 * @param pol
		 * @return
		 */
		int setClockPolarity(uint8_t pol);

		/**
		 *  Function sets active edge for the clock signal.
		 * @param phase
		 * @return
		 */
		int setClockPhase(uint8_t phase);

		/**
		 * Function sets order of bits in transmitted data.
		 * @param lsb_first
		 * @return
		 */
		int setLSBFirst(bool lsb_first);

		/**
		 * Function sets the number of bits per transferred word.
		 * @param bits
		 * @return
		 */
		int setBitsPerWord(int bits);

		/**
		 * Function set the speed of the SPI interface.
		 * @param speed
		 * @return
		 */
		int setSpeed(uint32_t speed);


		/**
		 * Method writes len bytes to the device.
		 * @param wbuf
		 * @param len
		 * @return
		 */
		int write(const void *wbuf, int len);

		/**
		 * Method reads len bytes from the device.
		 * @param rbuf
		 * @param len
		 * @return
		 */
		int read(void *rbuf, int len);

		/**
		 * Function performs simultaneous read and write on the device.
		 * @param wbuf
		 * @param rbuf
		 * @param len
		 * @return
		 */
		int xfer1(const void *wbuf, void *rbuf, int len);

		/**
		 *
		 */
		virtual ~SPI();
};

#endif /* SPI_H_ */
