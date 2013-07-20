## Process this file with automake to produce Makefile.in
## ./src/Makefile.am

AM_CPPFLAGS=-I${srcdir}/../include/  -D_HW_PLATFORM_BEAGLEBONE -DDEBUG_LEVEL=$(DEBUG_LEVEL)
AM_LDFLAGS=-Llib
AM_ARFLAGS=rvs

lib_LIBRARIES = libgpiooo.a
libgpiooo_a_SOURCES = SPI.cpp GPIOoo.cpp BeagleGoo.cpp BeagleGooP.cpp HD44780.cpp HD44780gpioPhy.cpp TLC5946phy.cpp TLC5946chain.cpp TLC5946PRUSSphy.cpp JDT18003T01.cpp ST7735.cpp ST7735phy.cpp
