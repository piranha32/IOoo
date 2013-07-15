/*
 * ST7735phy.h
 *
 *  Created on: Jul 14, 2013
 *      Author: jacek
 */

#ifndef ST7735PHY_H_
#define ST7735PHY_H_

#include <stdint.h>

class ST7735phy
{
	public:

		ST7735phy();

		/**
		 *
		 */
		virtual ~ST7735phy();

		/**
		 *
		 * @param reset
		 */
		virtual void setReset(uint8_t reset)=0;

		/**
		 *
		 * @param datamode
		 */
		virtual void setDataMode(bool datamode)=0;

		/**
		 *
		 * @param wbuf
		 * @param len
		 * @return
		 */
		virtual int send(uint8_t wbuf[], int len)=0;
};

#endif /* ST7735PHY_H_ */
