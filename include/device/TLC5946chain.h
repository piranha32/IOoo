/*
 * TLC5946.h
 *
 *  Created on: Jun 3, 2013
 *      Author: jacek
 */

#ifndef TLC5946_H_
#define TLC5946_H_

#include "TLC5946phy.h"

class TLC5946chain
{
	private:
		TLC5946phy *phy;
		uint16_t *brightness;
		bool brightness_changed;

		uint8_t *dot;
		bool dot_changed;

		int chain_length;
	public:
		TLC5946chain(TLC5946phy *_phy, int num);
		virtual ~TLC5946chain();

		void setBrightness(int i, uint16_t b);
		void setDOT(int i, uint16_t dot);
		void blank(int b);
		void commit();
};

#endif /* TLC5946_H_ */
