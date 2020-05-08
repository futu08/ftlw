obj-m += iocontrol.o

KDIR = /usr/src/linux-headers-5.3.0-51-generic

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.mod *.symvers *.order
