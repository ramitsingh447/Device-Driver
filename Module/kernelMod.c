/**************************************************************
* Class: CSC-415-Spring2022
* Name:Ramit Singh
* Student ID:918213925
* GitHub ID:ramitsingh447
* Project: Assignment 6 - Device Driver
*
* File: kernelMod.c
*
* Description: A kernelMod that is connected with singh_ramit_HW6.main
*
* Template followed from: 
* http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
*
**************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "kernelmod"
#define CLASS_NAME "kmod"
#define WR_VALUE _IOW('a', 'a', int)
#define RD_VALUE _IOR('a', 'b', int)

MODULE_LICENSE("GPL");

static int majorNumber; // stores integer value of major number
static char message[256] = {0};
static short size_of_message;
static int value = 0; // integer value of 0
static int fibResult = 0;
static struct class *kernelmodClass = NULL;
static struct device *kernelmodDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int dev_fibonacci(int n);

static struct file_operations fops =
    {
        .open = dev_open,
        .read = dev_read,
        .write = dev_write,
        .unlocked_ioctl = dev_ioctl,
        .release = dev_release,
};

static int __init kernelmod_init(void)
{
   printk(KERN_INFO "KernelMod: Initializing the KernelMod LKM\n"); // intitalize the kernelmod

   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   
   if (majorNumber < 0)
   {
      printk(KERN_ALERT "KernelMod: Failed to register a major number\n"); // failed to register major number
      return majorNumber;
   }
   
   printk(KERN_INFO "KernelMod: Registered correctly with major number %d\n", majorNumber); // registered correctly with major number

   kernelmodClass = class_create(THIS_MODULE, CLASS_NAME);
   
   if (IS_ERR(kernelmodClass))
   { 
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "KernelMod: Failed to register device class\n"); // failed to register the device class
      return PTR_ERR(kernelmodClass);
   }
   
   printk(KERN_INFO "KernelMod: Device class registered correctly\n"); // shows device class registred correctly

   kernelmodDevice = device_create(kernelmodClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   
   if (IS_ERR(kernelmodDevice))
   {                                 
      class_destroy(kernelmodClass); // destroys kernelmodClass
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "KernelMod: Failed to create the device\n"); // shows kernel mod is failed to create the device
      return PTR_ERR(kernelmodDevice);
   }
   
   printk(KERN_INFO "KernelMod: Device class created correctly\n"); // right device class created correctly
   
   return 0;
}

static void __exit kernelmod_exit(void)
{
   device_destroy(kernelmodClass, MKDEV(majorNumber, 0)); // device is destroyed
    class_unregister(kernelmodClass); // class is unregistered from kernelmodClass                  
    class_destroy(kernelmodClass);  // class is destroyed from kernelmodClass                      
    unregister_chrdev(majorNumber, DEVICE_NAME);           
    printk(KERN_INFO "KernelMod: Goodbye from the LKM\n"); // prints goodbye from the LKM
 }

static int dev_open(struct inode *inodep, struct file *filep) //string consiting of names 
{
   printk(KERN_INFO "KernelMod: Device opened\n"); //kernelmod device is opened
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
   int error_count = 0;
   
   error_count = copy_to_user(buffer, message, size_of_message);

   if (error_count == 0)
   { 
      printk(KERN_INFO "KernelMod: Sent %d characters to the user\n", size_of_message); // to print out decimal number to the user
      return (size_of_message = 0); // sends certian number to user
   }
   else
   {
      printk(KERN_INFO "KernelMod: Failed to send %d characters to the user\n", error_count); // fails to send certian number to user
      return -EFAULT; 
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
   sprintf(message, "%s", buffer); // prints string buffer
    size_of_message = strlen(message); // string length of message
    printk(KERN_INFO "KernelMod: Received %zu characters from the user\n", len); // prints received from the size_t length of characters from user
    return len; 
 }

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
   switch (cmd)
   {
   case WR_VALUE:
      copy_from_user(&value, (int32_t*) arg, sizeof(value));
      printk(KERN_INFO "KernelMod: Received value %d from the user\n", value); // prints the decimal value from the user
      break;
   case RD_VALUE:
      fibResult = dev_fibonacci(value);
      copy_to_user((int32_t*) arg, &fibResult, sizeof(fibResult));
      printk(KERN_INFO "KernelMod: Sent Fibonacci result %d to the user\n", fibResult); // prints out the results of decimal sent to the Fibonacci result
      break;
   }

   return 0;
}

static int dev_fibonacci(int n)
{
   if (n <= 1)
   {
      return n;
   }
   return dev_fibonacci(n - 1) + dev_fibonacci(n - 2);
}

static int dev_release(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "KernelMod: Device successfully closed\n"); // prints out that the device has sucuessfully closed
   return 0;
}

module_init(kernelmod_init); // used to make the fucntion to be used as the entry point
module_exit(kernelmod_exit); // only will be called if the module usage has reached to zero
