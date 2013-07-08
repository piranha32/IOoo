CROSS=/usr/local/oecore-x86_64/sysroots/x86_64-angstromsdk-linux/usr/bin/armv7a-vfp-neon-angstrom-linux-gnueabi/arm-angstrom-linux-gnueabi-


CPPFLAGS=-I../include/  -D_HW_PLATFORM_BEAGLEBONE 
CXXFLAGS=-O0 -g -pedantic -Wall -std=gnu++11
CFLAGS=-pedantic -Wall 
CXX=${CROSS}g++ 
CC=${CROSS}gcc
LD=${CROSS}g++ 
LDFLAGS=-L../lib
MAKEDEPEND = $(CXX) -M $(CPPFLAGS) -o $(df).d $<
PRULIBS=-lprussdrv -lpthread
LDLIBS=-lgpiooo

vpath %.h include 

default: ../bin/gpio_lcd ../bin/gpio_leds ../bin/tlc5946 ../bin/tlc5946_clock.bin

%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

../bin/gpio_lcd: gpio_lcd.o TestLCD.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS)

../bin/gpio_leds: gpio_leds.o TestGPIOLeds.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS)

../bin/tlc5946: tlc5946.o TestTLC5946.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS) $(PRULIBS)

../bin/tlc5946_clock.bin: pru/clock.p
	 pasm -lbz $^ $(patsubst %.bin,%,$@)

doc:
	doxygen doc/ooperipherial.doxyfile 
pdf: doc
	make -C doc/doxygen/latex/
	make -C doc/doxygen/latex/
	cp doc/doxygen/latex/refman.pdf doc/iooo.pdf
clean:
	rm -f ../bin/tlc5946_clock.bin ../bin/gpio_lcd *.o

