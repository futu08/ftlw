obj-m += iocontrol2.o

KDIR = /usr/src/linux-headers-$(uname -r)

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.mod *.symvers *.order
