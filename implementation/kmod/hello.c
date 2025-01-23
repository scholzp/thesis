#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/fs.h>
#include <asm/e820/api.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

void test(void);
int get_bash_result(char *filename);

const size_t BOOTCODE_ADDRESS = START_ADDRESS * 1024;

int init_module(void)
{
	
	pr_info("***KMOD: Hello world 1.\n");
	
	test();
	pr_info("***KMOD: Using address 0x%016lx\n", BOOTCODE_ADDRESS);
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