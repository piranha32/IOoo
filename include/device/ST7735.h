/*
 * ST7735.h
 *
 *  Created on: Jul 14, 2013
 *      Author: jacek
 */

#ifndef ST7735_H_
#define ST7735_H_

#include <stdint.h>
#include "ST7735phy.h"

#ifndef _BV
#define _BV(x) (1<<(x))
#endif

class ST7735
{
	protected:
		ST7735phy *phy;
		bool active;

	public:
		enum ST7735commands
		{
			NOP=0x00,
			SWRESET=0x01,
			RDDID=0x04,
			RDDST=0x09,
			RDDPM=0x0a,
			RDDMADCTL=0x0b,
			RDDCOLMOD=0x0c,
			RDDIM=0x0d,
			RDDSM=0x0e,
			SLPIN=0x10,
			SLPOUT=0x11,
			PTLON=0x12,
			NORON=0x13,
			INVOFF=0x20,
			INVON=0x21,
			GAMSET=0x26,
			DISPOFF=0x28,
			DISPON=0x29,
			CASET=0x2a,
			RASET=0x2b,
			RAMWR=0x2c,
			RGBSET=0x2d,
			RAMRD=0x2e,
			PTLAR=0x30,
			TEOFF=0x34,
			TEON=0x35,
			MADCTL=0x36,
			IDMOFF=0x38,
			IDMON=0x39,
			COLMOD=0x3a,
			RDID1=0xda,
			RDID2=0xdb,
			RDID3=0xdc
		};

		//Bit values for RDDST
		static const uint8_t RDDST_BSTON=_BV(7);
		static const uint8_t RDDST_MY=_BV(6);
		static const uint8_t RDDST_MX=_BV(5);
		static const uint8_t RDDST_MV=_BV(4);
		static const uint8_t RDDST_ML=_BV(3);
		static const uint8_t RDDST_RGB=_BV(2);
		static const uint8_t RDDST_MH=_BV(1);

		static const uint8_t RDDST_IFP2=_BV(6);
		static const uint8_t RDDST_IFP1=_BV(5);
		static const uint8_t RDDST_IFP0=_BV(4);
		static const uint8_t RDDST_IDMON=_BV(3);
		static const uint8_t RDDST_PTLON=_BV(2);
		static const uint8_t RDDST_SLOUT=_BV(1);
		static const uint8_t RDDST_NORON=_BV(0);

		static const uint8_t RDDST_INVON=_BV(5);
		static const uint8_t RDDST_DISON=_BV(2);
		static const uint8_t RDDST_TEON=_BV(1);
		static const uint8_t RDDST_GCS2=_BV(0);

		static const uint8_t RDDST_GCS1=_BV(7);
		static const uint8_t RDDST_GCS0=_BV(6);
		static const uint8_t RDDST_TEM=_BV(5);




		ST7735(ST7735phy *phy);
		virtual ~ST7735();

		virtual void init();
		virtual void reset();

		virtual void setSleep(bool sleep);
		virtual void setInverse(bool inv);
		virtual void setGamma(int gamma);
		virtual void setIdle(bool idle);
		virtual void on();
		virtual void off();





};

#endif /* ST7735_H_ */
