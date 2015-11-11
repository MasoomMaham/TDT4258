/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/fs.h> //?
#include <linux/ioport.h> 
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/moduleparam.h> //?
#include <asm/uaccess.h>
#include <asm/signal.h> //?
#include <asm/siginfo.h> //?
#include <asm/io.h>
#include "driver-gamepad.h"

#define DRIVER_NAME "driver_name"
#define CLASS_NAME "gamepad_class"
#define GPIO_EVEN_NUMBER 17
#define GPIO_ODD_NUMBER 18
#define DEV_NUM_COUNT 1

static int __init gamepad_init(void);
static void __exit gamepad_exit(void);
static int gamepad_open(struct inode*, struct file*);
static int gamepad_release(struct inode*, struct file*);
static ssize_t gamepad_read(struct file*, char* __user, size_t, loff_t*);
static ssize_t gamepad_write(struct file*, char* __user, size_t, loff_t*);
static irqreturn_t gpio_interrupt_handler(int, void*, struct pt_regs*);

static dev_t dev_num;

struct cdev *gamepad_cdev;
struct class *cl;
void __iomem *gpio_mem;
void __iomem *portc_mem;

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static struct file_operations gamepad_fops = {
	.owner = THIS_MODULE, 
	.read = gamepad_read, 
	.write = gamepad_write, 
	.open = gamepad_open, 
	.release = gamepad_release 
};

/* user program reads from the driver */ 
static ssize_t gamepad_read (struct file* filp , char __user* buffer , size_t count , loff_t* offp )
{
	uint32_t buttons;
	printk(KERN_INFO "Reading buttons from gamepad driver.\n");
	/*copies buttonstatus from kernelspace to userspace*/
	buttons = ioread32(GPIO_PC_DIN);
	copy_to_user(buffer, &buttons, 1);
	return 1;
} 

static ssize_t gamepad_write(struct file* filp, char* __user buffer, size_t count, loff_t* offp)
{
	printk(KERN_INFO "Cannot write to buttons.\n");
	return 1;
}

static ssize_t gamepad_open(struct inode* inode , struct file* filp )
{
	printk(KERN_INFO "Gamepad driver opened.\n");
	return 1;
}

static ssize_t gamepad_release(struct inode* inode , struct file* filp )
{
	printk(KERN_INFO "Gamepad driver closed.\n");
	return 1;
}

/*interrupt handler*/
irqreturn_t gpio_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs)
{
	printk(KERN_ALERT "GPIO interrupt handeling.\n");
	iowrite32(0xff, gpio_mem+GPIO_IFC);
//TO DO: asyncronic queue
	return IRQ_HANDLED;
}

static int __init gamepad_init(void)
{
	int number;
	printk(KERN_INFO "Initializing gamepad driver\n");
	
	/*Allocating device number*/
	printk(KERN_INFO "Allocating device number.\n");
	number = alloc_chrdev_region(&dev_num, 0, DEV_NUM_COUNT, DRIVER_NAME); 
	if (number < 0) {
		printk(KERN_ALERT "Failed to allocate device number.\n");
		return -1;
	}
	
	printk(KERN_INFO "Requesting IO access.\n");
	if (request_mem_region(GPIO_PC_BASE, 0x24, DRIVER_NAME) == NULL ) {
		printk(KERN_ALERT "Error requesting GPIO_PC_MODEL memory region, already in use?\n");
		return -1;
	}
	/*if (request_mem_region(GPIO_PC_DOUT, 1, DRIVER_NAME) == NULL ) {
		printk(KERN_ALERT "Error requesting GPIO_PC_DOUT memory region, already in use?\n");
		return -1;
	}
	if (request_mem_region(GPIO_PC_DIN, 1, DRIVER_NAME) == NULL ) {
		printk(KERN_ALERT "Error requesting GPIO_PC_DIN memory region, already in use?\n");
		return -1;
	}*/

	gpio_mem = ioremap_nocache(GPIO_PA_BASE+0x100, 0x20);
	printk(KERN_DEBUG "gpio_mem addr: %p\n", gpio_mem);
	if (gpio_mem == NULL) printk(KERN_ERR "GPIO remap failed\n");
	portc_mem = ioremap_nocache(GPIO_PC_BASE, 0x24);
	printk(KERN_DEBUG "portc_mem addr: %p\n", portc_mem);
	if (portc_mem == NULL) printk(KERN_ERR "GPIO port c remap failed\n");

	printk(KERN_INFO "Activating buttons.\n");
	iowrite32(0x33333333, portc_mem+GPIO_PC_MODEL); //Activating pins to recognise input from buttons.
	iowrite32(0xFF, portc_mem+GPIO_PC_DOUT);	//Enable internal pull up.
	
	/* Initializing interrups*/
	printk(KERN_INFO "Initializing interrupts.\n");
	iowrite32(0x22222222, gpio_mem+GPIO_EXTIPSELL);
	request_irq(GPIO_EVEN_NUMBER, (irq_handler_t)gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);
	request_irq(GPIO_ODD_NUMBER, (irq_handler_t)gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);

	/* Initialize cdev */
	printk(KERN_INFO "Initializing cdev.\n");
	gamepad_cdev = cdev_alloc();
	gamepad_cdev->owner = THIS_MODULE;
	gamepad_cdev->ops = &gamepad_fops;
	cdev_add(&gamepad_cdev, dev_num, DEV_NUM_COUNT);
	
	/* Create class and device */
	printk(KERN_INFO "Creating device.\n");
	cl = class_create (THIS_MODULE, CLASS_NAME); 
	device_create ( cl , NULL, dev_num , NULL, CLASS_NAME);
	
	/* Enable interrutpts */
	iowrite32(0xff, gpio_mem+GPIO_EXTIFALL);
	iowrite32(0xff, gpio_mem+GPIO_EXTIRISE);
	iowrite32(0xff, gpio_mem+GPIO_IEN );
	
	printk(KERN_INFO "Gamepad driver initialized.\n");
	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit gamepad_exit(void)
{
	 printk(KERN_INFO "Exiting gamepad driver\n");

	/* Disable interrupts */
	printk(KERN_INFO "Disableing interrupst.\n");
	iowrite32(0x0000, gpio_mem+GPIO_IEN);
	free_irq(GPIO_EVEN_NUMBER, &gamepad_cdev);
	free_irq(GPIO_ODD_NUMBER, &gamepad_cdev);

	/* Release memory */
	// Could have requested memory space in init(), but we do not need to because there is only one device.
	printk(KERN_INFO "Releasing memoryspace.\n");
	release_mem_region(GPIO_PC_BASE, 0x24);

	
	/* Remove device */
	printk(KERN_INFO "Removing device.\n");
	device_destroy(cl, dev_num);
	class_destroy(cl);
	cdev_del(gamepad_cdev);
	unregister_chrdev_region(dev_num, DEV_NUM_COUNT);

	printk(KERN_INFO "Gamepad driver exited.\n");
}

module_init(gamepad_init);
module_exit(gamepad_exit);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

