#pragma once
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

int setup_tee_poll_timer(u8 *shared_mem_ptr);
void cleanup_tee_poll_timer(void);