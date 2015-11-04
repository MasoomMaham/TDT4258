/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h> 
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>

#define DRIVER_NAME "driver_name"
#define GPIO_EVEN_NUMBER 17
#define GPIO_ODD_NUMBER 18
#define DEV_NUM_COUNT 1

struct dev_t dev_num;
struct cdev gamepad_cdev;

static int __init gamepad_init(void);
static void __exit gamepad_exit(void);
static int gamepad_open(struct inode*, struct file*);
static int gamepad_release(struct inode*, struct file*);
static ssize_t gamepad_read(struct file*, char* __user, size_t, loff_t*);
static ssize_t gamepad_write(struct file*, char* __user, size_t, loff_t*);
static irqreturn_t gpio_interrupt_handler(int, void*, struct pt_regs*);

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static struct file operations my fops = {
	.owner = THIS MODULE, 
	.read = gamepad_read, 
	.write = gamepad_write, 
	.open = gamepad_open, 
	.release = gamepad_release 
};

/* user program reads from the driver */ 
static ssize_t gamepad_read (struct file* filp , char __user* buffer , size_t count , loff_t* offp )
{
	/*copies buttonstatus from kernelspace to userspace*/
	uint32_t buttons = ioread32(GPIO_PC_DIN);
	copy_to_user(buffer, &buttons, 1);
	return 1;
} 

static ssize_t gamepad_write(struct file* filp, char* __user buffer, size_t count, loff_t* offp)
{
	printk(KERN_INFO "Cannot write to buttons.");
	return 1;
}

static ssize_t gamepad_open(struct inode* inode , struct file* filp )
{
	printk(KERN_INFO "Kernel opened.");
	return 1;
}

static ssize_t gamepad_release(struct inode* inode , struct file* filp )
{
	printk(KERN_INFO "Kernel closed.");
	return 1;
}

/*interrupt handler*/
irqreturn_t gpio_interrupt_handler(int irq, void* dev_id, struct pt_regs* regs)
{
	printk(KERN_ALERT "GPIO interrupt handeling.\n");
	iowrite32(ioread32(GPIO_IF), GPIO_IFC);
	return IRQ_HANDLED;
}

static int __init template_init(void)
{
	printk("Hello World, here is your module speaking\n");
	return 0;
	
	/*Allocating device number*/
	number = alloc_chrdev_region(&dev_num, 0, DEV_NUM_COUNT, DRIVER_NAME); 
	if (result < 0) {
		printk(KERN_ALERT "Failed to allocate device number\n");
		return -1;
	}
	
	/*
	iowrite32(0x33333333, GPIO_PC_MODEL);
	iowrite32(0xFF, GPIO_PC_DOUT);
	*/
	*GPIO_PC_MODEL = 0x33333333; //Activating pins to recognise input from buttons.
  	*GPIO_PC_DOUT = 0xFF;	     //Enable internal pull up.
	
	
	request irq(GPIO_EVEN_NUMBER, gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);
	request irq(GPIO_ODD_NUMBER, gpio_interrupt_handler, 0, DRIVER_NAME, &gamepad_cdev);

	
	
	
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
	 printk("Short life for a small module...\n");
	
	/* Disable interrupts */
	iowrite32(0x0000, GPIO_IEN);
	free_irq(GPIO_EVEN_NUMBER, &gamepad_cdev);
	free_irq(GPIO_ODD_NUMBER, &gamepad_cdev);

	unregister_chrdev_region(dev_num, DEV_NUM_COUNT);
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

