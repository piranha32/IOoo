## Process this file with automake to produce Makefile.in
## ./src/Makefile.am

DEBUG_LEVEL=@DEBUG_LEVEL@
AM_CPPFLAGS=-I${top_srcdir}/include/ -D_HW_PLATFORM_BEAGLEBONE -DDEBUG_LEVEL=$(DEBUG_LEVEL)
AM_LDFLAGS=-Llib
AM_ARFLAGS=rvs

lib_LIBRARIES = libgpiooo.a
libgpiooo_a_SOURCES = I2C.cpp SPI.cpp GPIOoo.cpp BeagleGoo.cpp BeagleGooP.cpp ADC.cpp NativeADC.cpp BeagleADC.cpp LTC248X.cpp HD44780.cpp HD44780gpioPhy.cpp TLC5946phy.cpp TLC5946chain.cpp TLC5946PRUSSphy.cpp JDT18003T01.cpp ST7735.cpp ST7735phy.cpp