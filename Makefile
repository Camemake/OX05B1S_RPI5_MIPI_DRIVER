
obj-m += ox05b1s.o

ox05b1s-objs := src/ox05b1s.o src/ox05b1s_modes.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

install:
	sudo cp ox05b1s.ko /lib/modules/$(shell uname -r)/kernel/drivers/media/i2c/
	sudo depmod -a

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
