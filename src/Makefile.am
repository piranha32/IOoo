## Process this file with automake to produce Makefile.in
## ./src/Makefile.am

AM_CPPFLAGS=-I${top_srcdir}/include/ -D_HW_PLATFORM_BEAGLEBONE -D_KERNEL_VERSION=\"$(KERNEL_VERSION)\" -DDEBUG_LEVEL=$(DEBUG_LEVEL)

lib_LTLIBRARIES = libgpiooo.la
libgpiooo_la_LDFLAGS = -version-info $(MAJOR_VERSION):$(MINOR_VERSION)
libgpiooo_la_ARFLAGS = rvs
libgpiooo_la_SOURCES = I2C.cpp SPI.cpp GPIOoo.cpp BeagleGoo.cpp BeagleGooP.cpp ADC.cpp NativeADC.cpp BeagleADC.cpp EEPROM24CX.cpp HD44780.cpp HD44780gpioPhy.cpp TLC5946phy.cpp TLC5946chain.cpp JDT18003T01.cpp ST7735.cpp ST7735phy.cpp

if HAS_PRUSS
libgpiooo_la_LIBADD = -lprussdrv
libgpiooo_la_SOURCES += TLC5946PRUSSphy.cpp
endif