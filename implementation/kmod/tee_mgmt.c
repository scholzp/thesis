#include <asm/nmi.h>
#include <linux/sched.h>
#include "tee_mgmt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

char IRQ_VECTOR = 10;

static int tee_nmi_unknown_handler(unsigned int val, struct pt_regs *regs)
{
	pr_info("Executed %s\n", __FUNCTION__);
	return NMI_HANDLED;
}

int setup_tee_irq_handler(void) {
	int rc = -1;

	pr_info("Executed %s\n", __FUNCTION__);

	rc = register_nmi_handler(NMI_UNKNOWN, tee_nmi_unknown_handler, 0, "tee_notify");
	return rc; 
}
