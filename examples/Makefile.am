## Process this file with automake to produce Makefile.in
## ./examples/Makefile.am

AM_CPPFLAGS=-I../include/  -D_HW_PLATFORM_BEAGLEBONE #-DDEBUG=2
AM_CXXFLAGS=-O0 -g -pedantic -Wall -std=gnu++11
AM_LDFLAGS=-L../lib -L../src
LDADD=libgpiooo.a

noinst_PROGRAMS = gpio_lcd gpio_buttons gpio_leds test_jd-t18003-t01 # tlc5946 tlc5946_clock.bin


gpio_lcd_SOURCES: gpio_lcd.cpp TestLCD.cpp

gpio_leds_SOURCES: gpio_leds.cpp TestGPIOLeds.cpp

gpio_buttons_SOURCES: gpio_buttons.cpp TestGPIOButtons.cpp

test_jd-t18003-t01_SOURCES: test_jd-t18003-t01.cpp

#tlc5946_SOURCES: tlc5946.cpp TestTLC5946.cpp

#tlc5946_clock_bin_SOURCES: pru/clock.p
#tlc5946_clock_bin_LDADD = libprussdrv.a libpthread

#	 pasm -lb $^ $(patsubst %.bin,%,$@)

