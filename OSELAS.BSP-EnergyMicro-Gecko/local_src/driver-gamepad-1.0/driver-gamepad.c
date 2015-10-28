/*
 * This is a demo Linux kernel module.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h> 

#define DRIVER_NAME "driver_name"

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init template_init(void)
{
	printk("Hello World, here is your module speaking\n");
	return 0;
	
	/*
	ID = alloc_chrdev_region();
	*/

	req = request_mem_region(GPIO_PC_BASE, 36, DRIVER_NAME);
	if (!req){
	printk(KERN INFO ”Memory request denied.”);
	return -1; 	
	}
	ioremap nocache(GPIO_PC_DIN, 8);
	
	*GPIO_PC_MODEL = 0x33333333; //Activating pins to recognise input from buttons.
  	*GPIO_PC_DOUT = 0xFF;	     //Enable internal pull up.
	
	/*
	request irq();
	*/
	
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
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

