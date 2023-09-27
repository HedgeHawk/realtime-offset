obj-m += realtime_offset.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
test:
	cp realtime_offset.ko /tmp/
	-sudo rmmod realtime_offset
	sudo insmod /tmp/realtime_offset.ko
	dmesg