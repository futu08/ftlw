#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>		//file_operations structure- which of course allows use to open/close, read/write to device
#include <linux/cdev.h> 	//this is a chr driver; makes cdev available
#include <linux/semaphore.h> 	//used to access semaphores; synchronization behaviors
#include <linux/uaccess.h> 	//copy_to_user; copy_from_user
#include <linux/ioctl.h>


#define GETVALUE1 1
#define SETVALUE1 2
#define GETVALUE2 3
#define SETVALUE2 4


struct fake_device {
	char data_1[100];
	char data_2[100];
	struct semaphore sem; 
}virtual_device;


static long driver_ioctl(struct file *instance, unsigned int cmd, unsigned long arg){	
	int nc; 
	switch(cmd){
		case GETVALUE1:	
			printk(KERN_ALERT "IOCONTROL: Copying data_1 buffer to userspace\n"); 
			nc = copy_to_user((void*)arg, virtual_device.data_1, strlen(virtual_device.data_1)+1); 	
			break;
		case SETVALUE1: 
			printk(KERN_ALERT "IOCONTROL: Coyping buffer from userspace to data_1\n"); 
			nc = copy_from_user(virtual_device.data_1, (void*)arg, 100); 
			break;
		case GETVALUE2:	
			printk(KERN_ALERT "IOCONTROL: Copying data_2 buffer to userspace\n"); 
			nc = copy_to_user((void*)arg, virtual_device.data_2, strlen(virtual_device.data_2)+1); 
			break;
		case SETVALUE2:
			printk(KERN_ALERT "IOCONTROL: Coyping buffer from userspace to data_2\n"); 
			nc = copy_from_user(virtual_device.data_2, (void*)arg, 100);  
			break;
		default:
			printk(KERN_ALERT "IOCONTROL: Missing arguments\n");
			return -EINVAL;
		return 0;
	}
}


static struct cdev *mcdev; 	//m stand 'my'
int major_number;	//will store our major number- extracted from from dev_t using macro - mknod /director/file c major minor
int ret;		//will be used to hold return values of functions; this is beacause the kernel stack is very small
			//so delaring variables all over the pass in our module functions eats up the stack very fast	
static dev_t dev_num;		//will hold major number that kernel gives us
static struct class *my_class;			//name-->appears in /proc/devices
static struct device *my_device;

#define DEVICE_NAME "fiocontrol"



/*(7)called on device_file open
//	inode reference to the file on disk
//	and contains information about that file
//	struct file is represents an abstract open file
int device_open(struct inode *inode, struct file *filp){
	
	//only allow one process to open this device by using a semaphore as mutual exclusive lock- mutex
	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "IOCONTROL: could not lock device during open\n");
		return -1;	
	}
	
	printk(KERN_INFO "IOCONTROL: opened device\n");
	return 0;
}
*/

/*(8) called when user wants to get information from the device
ssize_t device_read(struct file* filp, char *bufStoreData, size_t bufCount, loff_t* curOffset){
	//take data from kernel space(device) to user space (process)
	//copy_to_user (destination, source, sizeToTransfer)
	printk(KERN_INFO "soliduscode: Reading from device\n");
	ret = copy_to_user(bufStoreData,virtual_device.data, bufCount);
	return ret;
}
*/

/*(9) called when user wants to send information to the device
ssize_t device_write(struct file* filp, const char* bufSourceData, size_t bufCount, loff_t* curOffset){
	//send data from user to kernel
	//copy_from_user (dest, source, count)
	
	printk(KERN_INFO "soliduscode: writing to device\n");
	ret = copy_from_user(virtual_device.data, bufSourceData, bufCount);
	return ret; 
}
*/

/*(10) called upon user close
int device_close(struct inode *inode, struct file *filp){
	
	//by calling up, which is opposite of down for semaphore, we release the mutex that we optained at device open
	//this has the effect of allowing other process to use the device now
	up(&virtual_device.sem);
	printk(KERN_INFO "soliduscode: closed device\n");
	return 0;

}
*/

//HERE
//(6) Tell the kernel which functions to call when user operates on our device file
struct file_operations fops = {
	.owner = THIS_MODULE,		//prevent unloading of this module when operations are in use
	//.open = device_open,		//points to the method to call when opening the device
	//.release = device_close,	//points to the method to call when closing the device
	//.write = device_write,		//points to the method to call when writing to the device
	//.read = device_read,		//points to the method to call when reading from the device
	.unlocked_ioctl = driver_ioctl,
};

static int driver_entry(void){
	
	//(3) Register our device with the system: a two step process
	//step(1) use dynamic allocation to assign our device
	//	a major number -- alloc_chrdev_region(dev_t*, uint fminor, uint count, char *name)
	
	if(alloc_chrdev_region(&dev_num, 0,1, DEVICE_NAME) < 0){
		printk(KERN_ALERT "iocontrol: failed to allocate a major number\n");
		return -EIO; 	//progagate error
	}
	/*major_number = MAJOR(dev_num); //extracts the major number and store in our variables (MACRO)
	printk(KERN_INFO "soliduscode: major number is %d\n", major_number);
	printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file\n", DEVICE_NAME, major_number); //dmesg
	*/
	mcdev = cdev_alloc();
	if(mcdev == NULL){
		goto free_device_number;
	}
	mcdev->owner = THIS_MODULE;
	mcdev->ops = &fops;
	if(cdev_add(mcdev, dev_num, 1))
		goto free_cdev;

	my_class = class_create(THIS_MODULE, DEVICE_NAME);
	if(IS_ERR(my_class)){
		pr_err("iocontrol: no udev support\n");
		goto free_cdev;
	}
	my_device = device_create(my_class,NULL, dev_num, NULL, "%s", "fliocontrol");
	if(IS_ERR(my_device)){
		pr_err("iocontrol: device_create failed\n");
		goto free_class;
	}
	
	printk(KERN_ALERT "iocontrol: loaded module\n");
	sema_init(&virtual_device.sem, 1);
	return 0;
	

	free_class:
		class_destroy(my_class);
	free_cdev:
		kobject_put(&my_device->kobj);
	free_device_number:
		unregister_chrdev_region(dev_num,1);
		return -EIO;
}


static void driver_exit(void){
	//(5)	unregister everything in reverse order
	//(a)
	device_destroy(my_class, dev_num);
	class_destroy(my_class); 
	cdev_del(mcdev);

	//(b)
	unregister_chrdev_region(dev_num,1);
	printk(KERN_ALERT "iocontrol: unloaded module\n");
}


//In
module_init(driver_entry);
module_exit(driver_exit);

MODULE_LICENSE("GPL");

















