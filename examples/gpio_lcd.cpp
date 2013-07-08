//============================================================================
// Name        : BeagleboneSPI.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>

#include "GPIOoo.h"
#include "GPIOpin.h"
#include "device/HD44780gpioPhy.h"
#include "device/HD44780.h"
#include "TestLCD.h"
#include "debug.h"

using namespace std;


int main()
{
	TestLCD lcdTest(8);
	lcdTest.loop();
	return 0;
}
