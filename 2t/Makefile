obj-m :=main.o
all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insert: 
	insmod main.ko

remove:
	rmmod main.ko

makefs:
	for i in 1 2 5 6; do mkfs.vfat /dev/mydisk$$i; done

VP1=/dev/mydisk1
VP2=/dev/mydisk5
MP1=/tmp/p1
MP2=/tmp/p2
mount:
	mkdir -p $(MP1) $(MP2)
	mount $(VP1) $(MP1)
	mount $(VP2) $(MP2)


umount:
	umount $(MP1)
	umount $(MP2)
	rmdir $(MP1) $(MP2) 

bw_test: bw_test_int bw_test_ext

fill_bw_file:
	@dd if=/dev/zero of=$(MP1)/file bs=512 count=$$((5*1024*2*10)) &>/dev/null

bw_test_int: fill_bw_file
	dd if=$(MP1)/file of=$(MP2)/file bs=512 count=$$((5*1024*2*10)) status=progress

bw_test_ext: fill_bw_file
	dd if=$(MP1)/file of=/media/sf_inout/2t/file bs=512 count=$$((5*1024*2*10	)) status=progress
	@rm /media/sf_inout/2t/file

scenario: insert makefs mount bw_test umount remove

.PHONY: report
