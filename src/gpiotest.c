/*
 * gpiotest.c
 *
 *  Created on: Jun 3, 2013
 *      Author: jacek
 */

#define BUILD_TESTER
#ifdef BUILD_TESTER
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

void print_gpio_conf_info(char *name,unsigned int info)
{
	printf("%s: %02x: %4s %2s %4s %4s %i\n",
			name,
			info,
			((info & 0x40)?"slow":"fast"),
			((info & 0x20)?"rx":"  "),
			((info & 0x10)?"  up":"down"),
			((info & 0x40)?"    ":"pull"),
			info&0x7
			);
}

void print_gpio_infos(unsigned long *m_controlModule)
{
	print_gpio_conf_info("P9_16 (blank)",m_controlModule[0x84c/4]);
	print_gpio_conf_info("P9_15 (mode) ",m_controlModule[0x840/4]);
	print_gpio_conf_info("P9_23 (xhalf)",m_controlModule[0x844/4]);
	print_gpio_conf_info("P9_13 (xerr) ",m_controlModule[0x874/4]);
	print_gpio_conf_info("G1_21 (led0) ",m_controlModule[(0x800+1*32+21)/4]);
}


//blank_pin_addr = 0x84c; //P9_16 -> 0x04c (output) gpio1_19
//mode_pin_addr  = 0x840; //P9_15 -> 0x040 (output) gpio1_16
//xhalf_pin_addr = 0x844; //P9_23 -> 0x044 (output) gpio1_17
//xerr_pin_addr  = 0x874; //P9_13 -> 0x074 (input) gpio0_31

#define DATA_OUT_REG 0x13C
#define GPIO_OE_REG 0x134
//spru73h pg 4093
#define DATA_CLEAR_REG  0x190
//spru73h pg 4094
#define DATA_SET_REG  0x194


static void test_gpio()
{
	//unsigned long GPIO_Control_Module_Registers = 0x44E10000;
	int gpio_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (gpio_fd < 0)
	{
		printf("Cannot open /dev/mem\n");
		return;
	}

	/*
	unsigned long *m_controlModule = (unsigned long *) mmap(NULL, 0x1FFF,
			PROT_READ | PROT_WRITE, MAP_SHARED, gpio_fd,
			GPIO_Control_Module_Registers);

	if (m_controlModule == MAP_FAILED )
	{
		printf("Control Module Mapping failed\n");
		return;
	}
*/
	unsigned long *gpio = (unsigned long *) mmap(NULL, 0xFFF,
			PROT_READ | PROT_WRITE, MAP_SHARED, gpio_fd, 0x4804C000);
	if (gpio <= 0)
	{
		printf("GPIO Mapping failed\n");
		return;
	}

	gpio[GPIO_OE_REG/4] &= ~((1<<16) | (1<<17) | (1<<19));
	unsigned int i = 0;
	for (;;)
	{


		//printf("\n---\nOE reg: %08lx\n",gpio[GPIO_OE_REG/4]);
		//print_gpio_infos(m_controlModule);
		printf("Loop %i, setting 0\n", i);
		gpio[DATA_SET_REG / 4] |= ((1<<16) | (1<<17) | (1<<19));;
		usleep(500000);
		printf("Loop %i, setting 1\n", i);
		gpio[DATA_CLEAR_REG / 4] |= ((1<<16) | (1<<17) | (1<<19));;
		usleep(500000);
		i++;
	}

}

int _main()
{
	test_gpio();
	return 0;
}

#endif
