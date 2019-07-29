obj-m	:=dynamic_eg.o
KDIR 	:=/home/paul/Panzer/4_14_linux
MYCC		=/home/paul/opt/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-

# The KDIR must have to assign a ./direction
modules:
	$(MAKE) ARCH=arm CROSS_COMPILE=$(MYCC) -C $(KDIR) SUBDIRS=$(shell pwd) modules
clean:
	rm -f modules.order Module.symvers  *.o *.ko *.mod.c
