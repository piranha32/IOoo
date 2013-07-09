#!/bin/bash

#workaround for a bug in bone-pinmux-helper to enable GPIO modules.

EXPORT=/sys/class/gpio/export
echo 5 > /sys/class/gpio/export
echo 65 > /sys/class/gpio/export
echo 105 > /sys/class/gpio/export

