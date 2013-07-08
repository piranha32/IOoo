

DTS=gpio_hd44780-00A0.dts gpio_leds-00A0.dts tlc5946-00A0.dts
DTBs=$(patsubst %.dts,../bin/%.dtb,$(DTS))

../bin/%.dtb: %.dts
	dtc -O dtb -o $@ -b 0 -@ $<

all: $(DTBs)

clean:
	rm -f  $(DTBs)

