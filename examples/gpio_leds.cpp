//============================================================================
// Name        : BeagleboneSPI.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "TestGPIOLeds.h"
#include "debug.h"

int main()
{

	TestGPIOLeds leds;
	leds.loop();
	return 0;
}
