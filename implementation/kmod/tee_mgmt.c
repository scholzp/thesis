#include <asm/nmi.h>
#include <linux/sched.h>

#include "tee_mgmt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

static struct shared_mem *SHARED_MEM_PTR = NULL;
static struct timer_list POLL_TIMER;

void ping_app(void);

static void tee_poll_timer_handler(struct timer_list *timer)
{
	if (NULL == SHARED_MEM_PTR) 
	{
		pr_err("%s: Shared memory pointer not initialized!\n", __FUNCTION__);
	}
	switch(SHARED_MEM_PTR->status) {
		case TEE_C_TEE_READY:
			pr_info("%s: TEE ready, send command!\n", __FUNCTION__);
			// For now we init the ping app
			SHARED_MEM_PTR->task_id = TEE_T_PING;
			SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
			break;
		case TEE_C_TEE_SEND:
			// delegate to respective app
			pr_info("%s: Received data from TEE!\n", __FUNCTION__);
			switch(SHARED_MEM_PTR->task_id) {
				case TEE_T_PING:
					ping_app();
					break;
				case TEE_T_UNKNOWN:
				default:
					pr_info("Unknow task ID: %u", SHARED_MEM_PTR->task_id);
					break;
			}
			break;
		// We don't need to do anything if our message was not processed yet
		case TEE_C_HOSTSEND:
		case TEE_C_NONE:
			pr_info("%s: No message pending!\n", __FUNCTION__);
			break;
		default:
			pr_info("%s:Received message with unknown code!\n", __FUNCTION__);
			break;
	}
	mod_timer(&POLL_TIMER, jiffies + msecs_to_jiffies(750));
}

int setup_tee_poll_timer(u8 *shared_mem_ptr) {
	pr_info("%s: Install NMI handler\n", __FUNCTION__);
	SHARED_MEM_PTR = (struct shared_mem*) shared_mem_ptr;
	timer_setup(&POLL_TIMER, tee_poll_timer_handler, 0);
	mod_timer(&POLL_TIMER, jiffies + msecs_to_jiffies(750));

	return 0; 
}

void cleanup_tee_poll_timer(void) {
	del_timer(&POLL_TIMER);
}

void ping_app(void) {
	const u8 number_of_pings = 10;
	// We simply use the first byte of the payload to count the pings
	if (number_of_pings > SHARED_MEM_PTR->memory[0]) {
		pr_info("Ping count = %u\n", SHARED_MEM_PTR->memory[0]++);
		pr_info("Pong");
		SHARED_MEM_PTR->task_id = TEE_T_PING;
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached ping count %u", SHARED_MEM_PTR->memory[0]);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}