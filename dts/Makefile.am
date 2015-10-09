DTC=@DTC@

DTS=gpio_hd44780-00A0.dts gpio_leds-00A0.dts tlc5946-00A0.dts gpio_buttons-00A0.dts jd-t18003-ctrl-00A0.dts
DTS+=BB-ADC-00A0.dts BB-I2C1-00A0.dts BB-I2C2-00A0.dts BB-SPI0-00A0.dts BB-SPI0-ALT-00A0.dts BB-SPI1-00A0.dts BB-SPI1-ALT-00A0.dts
DTBs=$(subst .dts,.dtbo,$(DTS))

%.dtbo: %.dts
	$(DTC) -O dtb -o $@ -b 0 -@ $<

all: $(DTBs)

clean-local:
	-rm -f *.dtbo
