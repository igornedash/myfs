#Default Makefile borrowed from LDD3 book for build, load and unload module skeleton
#Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

EXTRA_CFLAGS += -O2 -I.

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m += myfs.o
myfs-objs := inode.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)


modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif	
