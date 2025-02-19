#pragma once
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

#define IRQ_NAME "tee_irq_handler"

extern char IRQ_VECTOR;

int setup_tee_irq_handler(void);
extern irqreturn_t tee_irq_handler(int irq, void *dev_id);