DEBUG_LEVEL=3
CPPFLAGS=-I../include/  -D_HW_PLATFORM_BEAGLEBONE -DDEBUG_LEVEL=$(DEBUG_LEVEL)
CXXFLAGS=-O0 -g -pedantic -Wall -std=gnu++11
CFLAGS=-pedantic -Wall 
CXX=${CROSS}g++ 
CC=${CROSS}gcc
LD=${CROSS}g++ 
LDFLAGS=-Llib
AR=${CROSS}ar
ARFLAGS=rvs
vpath %.h ../include 
vpath %.cpp src 
vpath %.c src 

LIB_SRC= I2C.cpp SPI.cpp GPIOoo.cpp BeagleGoo.cpp BeagleGooP.cpp HD44780.cpp HD44780gpioPhy.cpp TLC5946phy.cpp TLC5946chain.cpp TLC5946PRUSSphy.cpp JDT18003T01.cpp ST7735.cpp ST7735phy.cpp
LIB_OBJS=$(patsubst %.cpp,%.o,$(LIB_SRC)) 

../lib/libgpiooo.a:$(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^
	



%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o ../bin/libgpiooo.a

