#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
 
typedef struct
{
    char buf1[100];
    char buf2[100];
} device_struct;

char safe_buf1[100] = {"das ist der buf1"};
char safe_buf2[100] = {"das ist der buf2"};

#define GET_BUFFER_1 _IOR('device_inf', 1, device_struct *)
#define SET_BUFFER_1 _IOW('device_inf', 2, device_struct *)
#define GET_BUFFER_2 _IOR('device_inf', 3, device_struct *)
#define SET_BUFFER_2 _IOW('device_inf', 4, device_struct *)
 
#define FIRST_MINOR 0
#define MINOR_CNT 1
 
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

 
static int my_open(struct inode *i, struct file *f)
{
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    return 0;
}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg) //wird kompiliert
#endif
{
    device_struct device_inf;
 
    switch (cmd)
    {
        case GET_BUFFER_1:
            strcpy(device_inf.buf1, safe_buf1);
            if (copy_to_user((device_struct*)arg, &device_inf, sizeof(device_inf)))
            {
                return -EACCES;
            }
            break;
        case SET_BUFFER_1:
            printk("sind in der SET_BUFFER_1\n");
            if (copy_from_user(&device_inf, (device_struct *)arg, sizeof(device_struct)))
            {
                return -EACCES;
            }
            strcpy(safe_buf1, device_inf.buf1);
            break;

        case GET_BUFFER_2:
            strcpy(device_inf.buf2, safe_buf2);
            if (copy_to_user((device_struct*)arg, &device_inf, sizeof(device_inf)))
            {
                return -EACCES;
            }
            break;
        case SET_BUFFER_2:
            
            if (copy_from_user(&device_inf, (device_struct *)arg, sizeof(device_struct)))
            {
                return -EACCES;
            }
            strcpy(safe_buf2, device_inf.buf2);
            break;


        default:
            return -EINVAL;
    }
 
    return 0;
}
 
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    .ioctl = my_ioctl
#else
    .unlocked_ioctl = my_ioctl
#endif
};
 
static int __init ioctl2_init(void)
{
    int ret;
    struct device *dev_ret;
 
 
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "Ioctl2")) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "ioctl2")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}
 
static void __exit ioctl2_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}
 
module_init(ioctl2_init);
module_exit(ioctl2_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Furkan Turan, Lukas Walczak");
MODULE_DESCRIPTION("Test Driver ioctl");