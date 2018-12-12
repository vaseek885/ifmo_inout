
PWD := $(shell pwd)
obj-m := virt1.o

default: all

all:
	sudo make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" modules
clean:
	sudo make -C /lib/modules/$(shell uname -r)/build M="$(PWD)" clean
do:
	sudo insmod ./virt1.ko link=enp0s3 port=10
od:
	sudo rmmod ./virt1.ko
if:
	ifconfig virt
redo:
	make od && make all && make do
	
