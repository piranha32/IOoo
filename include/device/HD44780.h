/*
 * HD44780.h
 *
 *  Created on: Jun 16, 2013
 *      Author: jacek
 */

#ifndef HD44780_H_
#define HD44780_H_

#include "HD44780phy.h"

class HD44780
{
	private:
		void _scrollUp();
		void _copyScreenToDisplay();
		void _gotoXY(uint8_t x, uint8_t y);
		virtual void writeData(uint8_t b);
		virtual void writeCmd(uint8_t b);
		virtual uint8_t readData();
		virtual uint8_t readStatus();
		virtual uint16_t computeAddress(int x, int y);

		HD44780phy *phy;
		uint8_t *screen;
		uint8_t cursorX, cursorY;
		uint16_t cols;
		uint16_t rows;
		int intSetupCmd;
	public:
		HD44780(HD44780phy *phy, int sizeX, int sizeY);
		virtual ~HD44780();

		/**
		 * \brief Function initializes the display
		 */
		void init();

		/**
		 * \brief Function clears the LCD and moves the cursor to home position
		 */
		void clear();

		/**
		 * \brief Function moves the cursor to home location
		 */
		void home();

		/**
		 * \brief Function moves the cursor to location (x,y)
		 */
		void gotoXY(uint8_t x, uint8_t y);

		/**
		 * \brief Function prints one character at current cursor position.
		 * If the cursor is at the end of the line, the
		 */
		void putcc(char c);

		/**
		 * \brief Function prints a string of characters at current cursor location.
		 * Control characters are not interpreted
		 */
		void puts(char *s);

		/**
		 * \brief Function prints a string of characters at current cursor location.
		 * Function interprets basic ANSI control characters:
		 * - Line feed '\\n'
		 * - Carriage return '\\r'
		 */
		void print(char *s);

		/**
		 * \brief Function defined a custom user character.
		 * \param c index of the character.
		 * \param def pointer to an array of 8 bytes holding definition of the character.
		 */
		void defineCustomCharacter(uint8_t c, uint8_t *def);

};

#endif /* HD44780_H_ */
