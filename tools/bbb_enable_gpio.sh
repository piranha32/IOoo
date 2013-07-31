#!/bin/bash

#workaround for a bug in bone-pinmux-helper to enable GPIO modules.
#based on the code from Luigi Rinaldi: https://groups.google.com/forum/#!msg/beagleboard/OYFp4EXawiI/Mq6s3sg14HoJ

EXPORT=/sys/class/gpio/export
echo 5 > /sys/class/gpio/export
echo 65 > /sys/class/gpio/export
echo 105 > /sys/class/gpio/export

