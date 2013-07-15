#include <stdio.h>
#include <unistd.h>
#include "device/ST7735.h"
#include "device/JDT18003T01.h"

#include "GPIOoo.h"
#include "GPIOpin.h"


SPI *setupSPI()
{
	SPI *spi = new SPI();
	printf("open: %d\n", spi->open(1, 1));
	//printf("setMode: %d\n", spi.setMode(SPI_CPHA));
	printf("setMode: %d\n", spi->setMode(0));
	printf("setSpeed: %d\n", spi->setSpeed(5000));

	return spi;
}

int main()
{
	SPI *spi = setupSPI();
	GPIOoo *gp = GPIOoo::getInstance();

	char *jdPins[] =
		{ "P8_19", "P8_18" };
	char *jdNames[] =
		{ "rs", "reset" };

	GPIOpin *blockJD = gp->claim((char **) jdPins, 2);

	printf("Naming pins\n");
	blockJD->namePins((char **) jdNames);

	JDT18003T01 jdPhy(spi,blockJD);
	ST7735 st(&jdPhy);

	jdPhy.setDataMode(false);
	jdPhy.setDataMode(true);

	st.init();

	usleep(1000000);
	st.off();

	usleep(1000000);
	st.on();

	return 0;
}
