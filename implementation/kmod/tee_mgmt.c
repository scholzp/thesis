#include <asm/nmi.h>
#include <linux/sched.h>

#include "tee_mgmt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

static u8 *SHARED_MEM_PTR = NULL;
static struct timer_list POLL_TIMER;

static void tee_poll_timer_handler(struct timer_list *timer)
{
	if (NULL == SHARED_MEM_PTR) 
	{
		pr_err("%s: Shared memory pointer not initialized!\n", __FUNCTION__);
	}
	if (0x1 != SHARED_MEM_PTR[0]) {
		pr_info("%s: No message pending!\n", __FUNCTION__);
	}
	SHARED_MEM_PTR[0] = 0;
	mod_timer(&POLL_TIMER, jiffies + msecs_to_jiffies(750));
}

int setup_tee_poll_timer(u8 *shared_mem_ptr) {
	pr_info("%s: Install NMI handler\n", __FUNCTION__);
	SHARED_MEM_PTR = shared_mem_ptr;
	timer_setup(&POLL_TIMER, tee_poll_timer_handler, 0);
	mod_timer(&POLL_TIMER, jiffies + msecs_to_jiffies(750));

	return 0; 
}

void cleanup_tee_poll_timer(void) {
	del_timer(&POLL_TIMER);
}
