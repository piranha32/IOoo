/*
 * HD44780.cpp
 *
 *  Created on: Jun 16, 2013
 *      Author: jacek
 */

#include "device/HD44780.h"
#include <unistd.h>

HD44780::HD44780(HD44780phy* phy, int sizeX, int sizeY)
{
	intSetupCmd = 0x2f; //4 bit interface, 2 lines, 5x10 font
	this->phy = phy;
	cursorX = 0;
	cursorY = 0;
	rows = sizeY;
	cols = sizeX;
	screen = new uint8_t[rows * cols];
}

HD44780::~HD44780()
{
	delete[] screen;
}

void HD44780::_scrollUp()
{
	int i, j, tmp;
	for (j = 0; j < rows - 1; j++)
	{
		tmp = j * cols;
		for (i = 0; i < cols; i++)
			screen[tmp + i] = screen[tmp + i + cols];
	}

	tmp = (rows - 1) * cols;
	for (i = 0; i < cols; i++)
		screen[tmp + i] = 0x20;

	_copyScreenToDisplay();
}

void HD44780::_copyScreenToDisplay()
{
	unsigned char i, j;

	for (i = 0; i < rows; i++)
	{
		int rowShift = i * cols;
		_gotoXY((unsigned char) 0, i);
		for (j = 0; j < cols; j++)
			writeData(screen[rowShift + j]);
	}

}

void HD44780::_gotoXY(uint8_t x, uint8_t y)
{
	unsigned char pos;
	pos = computeAddress(x, y);
	writeCmd(0x80 | pos);

}

void HD44780::writeData(uint8_t b)
{
	phy->setRS(HD44780phy::RSdata);
	phy->write(0, b);
}

void HD44780::writeCmd(uint8_t b)
{
	phy->setRS(HD44780phy::RScommand);
	phy->write(0, b);
}

uint8_t HD44780::readData()
{
	phy->setRS(HD44780phy::RSdata);
	return phy->read(0);
}

uint8_t HD44780::readStatus()
{
	phy->setRS(HD44780phy::RScommand);
	return phy->read(0);
}

void HD44780::init()
{

	if (phy->getBits() == 8)
	{
		intSetupCmd |= 0x10; //8 bit interface
	}
	writeCmd(intSetupCmd); //set function
	usleep(1000);
	//writeCmd(intSetupCmd); //set function
	//usleep(1000);

	writeCmd(0x0f); //display on
	usleep(1000);

	writeCmd(0x01); //clear
	usleep(10000);
	writeCmd(0x0f); //display on
	usleep(1000);
	writeCmd(0x02); //home
	usleep(10000);

	for (int j = 0; j < rows; j++)
		for (int i = 0; i < cols; i++)
			screen[j * cols + i] = 0x20;
	cursorX = 0;
	cursorY = 0;

}

void HD44780::clear()
{
	unsigned char i, j;

	writeCmd(0x01);

	for (j = 0; j < rows; j++)
		for (i = 0; i < cols; i++)
			screen[j * cols + i] = 0x20;
	cursorX = 0;
	cursorY = 0;
	usleep(10000);
}

void HD44780::home()
{
	writeCmd(0x02);
	usleep(10000);
	cursorX = 0;
	cursorY = 0;

}

void HD44780::gotoXY(uint8_t x, uint8_t y)
{
	if (x >= cols)
		x = cols - 1;
	if (y >= rows)
		y = cols - 1;

	cursorX = x;
	cursorY = y;
	_gotoXY(x, y);

}

void HD44780::putcc(char c)
{
	screen[cursorY * cols + cursorX] = c;
	writeData(c);

	if (++cursorX > cols - 1)
	{
		cursorX = 0;
		if (cursorY < rows - 1)
			cursorY++;
		else
		{
			_scrollUp();
			//no need to modify lcdCursorY - it's already in the last line
		}

		_gotoXY(cursorX, cursorY);
	}

}

void HD44780::puts(char* s)
{
	while (*s)
		putcc(*s++);
}

void HD44780::print(char* s)
{
	for (; *s; s++)
	{
		switch (*s)
		{
			case '\n': //line feed
				if (cursorY >= rows - 1)
					_scrollUp();
				else
					cursorY++;
				//fall through to carriage return
			case '\r':
				cursorX = 0;
				_gotoXY(cursorX, cursorY);
				break;
			default:
				putcc(*s);
		}
	}

}

uint16_t HD44780::computeAddress(int x, int y)
{
	uint16_t addr;
	switch (rows)
	{
		case 1:
			addr = x;
			break;
		case 2:
			addr = y * 0x40 * x;
			break;
		case 4:
			addr = x;
			if (y & 0x01)
				addr += 0x40;
			if (y & 0x02)
				addr += cols;
			break;
		default:
			addr = x;
	}
	return addr;
}

void HD44780::defineCustomCharacter(uint8_t c, uint8_t* def)
{
}
