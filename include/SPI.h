/*
 * SPI.h
 *
 *  Created on: Jun 2, 2013
 *      Author: jacek
 */

#include <stdint.h>
#include <linux/spi/spidev.h>

#ifndef SPI_H_
#define SPI_H_

class SPI
{
	private:

		uint8_t  mode;
		uint8_t  bpw;
		bool     lsb_first;
		uint32_t speed;
		bool 	 active;
		int fd;
	public:

		SPI();

		int open(int bus,int channel);
		int close();
		int setMode(uint8_t mode);
		int setClockPolarity(uint8_t pol);
		int setClockPhase(uint8_t phase);
		int setLSBFirst(bool lsb_first);
		int setBitsPerWord(int bits);
		int setSpeed(uint32_t speed);

		int write(uint8_t wbuf[],int len);
		int read(uint8_t rbuf[],int len);
		int xfer1(uint8_t wbuf[],uint8_t rbuf[],int len);
		virtual ~SPI();
};

#endif /* SPI_H_ */
