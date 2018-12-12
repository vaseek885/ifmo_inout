obj-m = ch_drv.o
PWD = $(shell pwd)

all:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules
	sudo rmmod ch_drv
	sudo insmod ch_drv.ko
	sudo chmod 777 /dev/var8
clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
