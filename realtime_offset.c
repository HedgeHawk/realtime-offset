#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include <linux/device.h>

#define DEVICE_NAME "realtime_offset"

static int    majorNumber;

static struct class*  offsetClass  = NULL;
static struct device* offsetDevice = NULL;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .release = dev_release,
};

static int __init offset_init(void){
   printk(KERN_INFO DEVICE_NAME " Initializing the Kernel module\n");

   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT DEVICE_NAME " failed to register a major number\n");
      return majorNumber;
   }

   offsetClass = class_create(THIS_MODULE, "realtime_offset_class");
   if (IS_ERR(offsetClass)){
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(offsetClass);
   }

   offsetDevice = device_create(offsetClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(offsetDevice)){
      class_destroy(offsetClass);
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(offsetDevice);
   }

   return 0;
}

static void __exit offset_exit(void){
   device_destroy(offsetClass, MKDEV(majorNumber, 0));
   class_unregister(offsetClass);
   class_destroy(offsetClass);
   unregister_chrdev(majorNumber, DEVICE_NAME);
   printk(KERN_INFO DEVICE_NAME " Module has been unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   try_module_get(THIS_MODULE); 
   printk(KERN_INFO DEVICE_NAME " Device has been opened\n");
   return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
   module_put(THIS_MODULE); 
   printk(KERN_INFO DEVICE_NAME " Device successfully closed\n");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   static char  time_value[1024] = {0};
   static short size_of_time=0;
   ktime_t mono=0;
   ktime_t mono2=0;
   ktime_t real=0;
   ktime_t offs=0;
   ktime_t offs1=0;
   ktime_t offs2=0;
   ktime_t offs3=0;

   if (*offset > 0) {
      // Offset set to non-zero means we've read the string already;
      // return 0 to indicate end-of-file
      return 0;
   }
   
   offs=ktime_mono_to_real(0);
   
   mono=ktime_get();
   real=ktime_get_real();
   mono2=ktime_get();
   
   offs1=real-mono;
   offs2=real-mono2;
   offs3=real-((mono+mono2)/2);

   size_of_time = snprintf(time_value, sizeof(time_value), "%lld %lld %lld %lld\n",offs, offs-offs1, offs-offs2, offs-offs3);

   error_count = copy_to_user(buffer, time_value, size_of_time);

   if (error_count == 0){
      printk(KERN_INFO DEVICE_NAME " Sent %d characters to the user\n", size_of_time);
      *offset += size_of_time;  // Update the offset
      return size_of_time;
   }
   else {
      printk(KERN_INFO DEVICE_NAME " Failed to send %d characters to the user\n", error_count);
      return -EFAULT;
   }
}

module_init(offset_init);
module_exit(offset_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple Linux driver for realtime offset");
MODULE_VERSION("0.1");
