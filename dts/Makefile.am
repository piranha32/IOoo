## Process this file with automake to produce Makefile.in
## ./dts/Makefile.am

.dts.dtbo:
	dtc -O dtb -o $@ -b 0 -@ $<

firmwaredir = /lib/firmware

firmware_PROGRAMS = gpio_hd44780-00A0.dtbo gpio_leds-00A0.dtbo tlc5946-00A0.dtbo gpio_buttons-00A0.dtbo spi1-00A0.dtbo jd-t18003-ctrl-00A0.dtbo

