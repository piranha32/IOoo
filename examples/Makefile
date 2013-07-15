CPPFLAGS=-I../include/  -D_HW_PLATFORM_BEAGLEBONE -DDEBUG=2
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

default: ../bin/gpio_lcd ../bin/gpio_buttons ../bin/gpio_leds ../bin/tlc5946 ../bin/tlc5946_clock.bin ../bin/test_jd-t18003-t01 

%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

../bin/gpio_lcd: gpio_lcd.o TestLCD.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS)

../bin/gpio_leds: gpio_leds.o TestGPIOLeds.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS)

../bin/gpio_buttons: gpio_buttons.o TestGPIOButtons.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS)

../bin/test_jd-t18003-t01: test_jd-t18003-t01.o 
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS)

../bin/tlc5946: tlc5946.o TestTLC5946.o
	${LD} $(LDFLAGS) $^ -o $@ $(LDLIBS) $(PRULIBS)

../bin/tlc5946_clock.bin: pru/clock.p
	 pasm -lb $^ $(patsubst %.bin,%,$@)

clean:
	rm -f ../bin/tlc5946_clock.* ../bin/gpio_lcd ../bin/tlc5946 ../bin/gpio_leds ../bin/test_jd-t18003-t01 ../bin/gpio_buttons *.o

