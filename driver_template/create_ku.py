import os
import pathlib
path = str(pathlib.Path().absolute())
list_dir = path.split("/")

driver_name = list_dir[-1].lower()
kernel_str = "kernel"
user_str = "user"
os.mkdir("kernel")
os.mkdir("user")

kernel_dir = os.path.join(path, kernel_str)
user_dir = os.path.join(path, user_str)

includes = """#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
"""

mf = """obj-m += {}.o

KDIR = /usr/src/linux-headers-5.3.0-51-generic

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -rf *.o *.ko *.mod.* *.mod *.symvers *.order

""".format(driver_name+"_driver")

includes_user_space = """#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
"""

kernel_c_file = open(kernel_dir + "/" + driver_name + "_driver.c", "w+")
kernel_c_file.write(includes)
kernel_c_file.close()

makefile = open(kernel_dir + "/" + "Makefile", "w+")
makefile.write(mf)
makefile.close()


user_c_file = open(user_dir + "/" + "userapp.c", "w+")
user_c_file.write(includes_user_space)
user_c_file.close()
