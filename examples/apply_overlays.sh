#!/bin/bash

slots=/sys/devices/bone_capemgr.9/slots

dtbs=gpio_hd44780 gpio_leds tlc5946
for a in $dtbs; do
  echo $a > $slots
done
