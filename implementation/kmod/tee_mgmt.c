#include <asm/nmi.h>
#include <linux/sched.h>
#include "tee_mgmt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

char IRQ_VECTOR = 10;
static u8 *SHARED_MEM_PTR = NULL;

static int tee_nmi_unknown_handler(unsigned int val, struct pt_regs *regs)
{
	if (NULL == SHARED_MEM_PTR) 
	{
		pr_err("%s: Shared memory pointer not initialized!\n", __FUNCTION__);
		return NMI_DONE;
	}
	if (0x1 != SHARED_MEM_PTR[0]) {
		pr_info("%s: No message pending!\n", __FUNCTION__);
		return NMI_DONE;
	}
	pr_info("%s: Ack message; Reset status field; Consume NMI...\n", __FUNCTION__);
	SHARED_MEM_PTR[0] = 0;

	return NMI_HANDLED;
}

int setup_tee_irq_handler(u8 *shared_mem_ptr) {
	int rc = -1;

	pr_info("%s: Install NMI handler\n", __FUNCTION__);
	SHARED_MEM_PTR = shared_mem_ptr;
	rc = register_nmi_handler(NMI_UNKNOWN, tee_nmi_unknown_handler, 0, "tee_notify");
	return rc; 
}
