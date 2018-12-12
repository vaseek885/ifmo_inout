obj-m = ram_block.o
ram_block-y := ram_device.o partition.o
PWD = $(shell pwd)

all:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules
	# sudo chmod 777 /dev/var8
clean:
	make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
