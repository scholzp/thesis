#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

void test(void);


MODULE_LICENSE("Proprietary");

int init_module(void)
{
	printk(KERN_INFO "Hello world 1.\n");
	test();

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

void test(void) {
	pr_info("we just called a different function\n");
}

void cleanup_module(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}