## Process this file with automake to produce Makefile.in
## ./examples/Makefile.am

AM_CPPFLAGS=-I${top_srcdir}/include/ -D_HW_PLATFORM_BEAGLEBONE
AM_LDFLAGS=-L../lib -L../src
LDADD = ../src/libgpiooo.a

noinst_PROGRAMS = gpio_lcd gpio_buttons gpio_leds test_jd-t18003-t01 # tlc5946 tlc5946_clock.bin

gpio_lcd_SOURCES = TestLCD.cpp gpio_lcd.cpp

gpio_leds_SOURCES = gpio_leds.cpp TestGPIOLeds.cpp

gpio_buttons_SOURCES = gpio_buttons.cpp TestGPIOButtons.cpp

test_jd_t18003_t01_SOURCES = test_jd-t18003-t01.cpp

#tlc5946_SOURCES = tlc5946.cpp TestTLC5946.cpp

#tlc5946_clock_bin_SOURCES = pru/clock.p
#tlc5946_clock_bin_LDADD = libprussdrv.a libpthread

#	 pasm -lb $^ $(patsubst %.bin,%,$@)

clean-local:
	-rm -f *.o