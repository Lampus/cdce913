#!/bin/bash
ARCH=arm CROSS_COMPILE=/opt/arm-2010q1/bin/arm-none-linux-gnueabi- make
/opt/arm-2010q1/bin/arm-none-linux-gnueabi-gcc -Wall calc_coeff.c -o calc_coeff.arm
/opt/arm-2010q1/bin/arm-none-linux-gnueabi-strip calc_coeff.arm
