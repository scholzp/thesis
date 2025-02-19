#include <linux/sched.h>
#include "tee_mgmt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

char IRQ_VECTOR = 10;

int setup_tee_irq_handler(void) {
	char currently_proping = IRQ_VECTOR;
	int rc = -1;

	pr_info("Executed %s\n", __FUNCTION__);

	while((0 != rc) && (255 > currently_proping)) {
		rc = request_irq(currently_proping, &tee_irq_handler, 0, "tee", NULL);
		++currently_proping;
	}
	pr_info("Return value: %d; Reserved vector %d", rc, currently_proping-1); 
	return rc; 
}

irqreturn_t tee_irq_handler(int irq, void *dev_id) {
	pr_info("Executed handler %s\n", __FUNCTION__);
	return IRQ_NONE;
}