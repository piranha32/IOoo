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

#include "TestGPIOButtons.h"

int main()
{
	TestGPIOButtons buttons;
	buttons.loop();
	return 0;
}
