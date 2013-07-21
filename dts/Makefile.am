

DTS=gpio_hd44780-00A0.dts gpio_leds-00A0.dts tlc5946-00A0.dts gpio_buttons-00A0.dts spi1-00A0.dts jd-t18003-ctrl-00A0.dts
DTBs=$(patsubst %.dts,%.dtbo,$(DTS))

%.dtbo: %.dts
	dtc -O dtb -o $@ -b 0 -@ $<

all: $(DTBs)

clean:
	rm -f  $(DTBs)

