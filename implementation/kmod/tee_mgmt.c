#include <asm/nmi.h>
#include <linux/sched.h>

#include "tee_mgmt.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

static struct shared_mem *SHARED_MEM_PTR = NULL;
static struct timer_list POLL_TIMER;
static u8 waiting = 0;

void ping_app(void);
void attack_read_mem(void);
void attack_write_mem(void);

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
			SHARED_MEM_PTR->task_id = TEE_T_ATTACK_WRITE_MEM;
			SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
			waiting = 0;
			break;
		case TEE_C_TEE_SEND:
			// delegate to respective app
			pr_info("%s: Received data from TEE!\n", __FUNCTION__);
			switch(SHARED_MEM_PTR->task_id) {
				case TEE_T_PING:
					ping_app();
					break;
				case TEE_T_ATTACK_READ_MEM:
					attack_read_mem();
					break;
				case TEE_T_ATTACK_WRITE_MEM:
					attack_write_mem();
					break;
				case TEE_T_UNKNOWN:
				default:
					pr_info("Unknow task ID: %u", SHARED_MEM_PTR->task_id);
					break;
			}
			waiting = 0;
			break;
		// We don't need to do anything if our message was not processed yet
		case TEE_C_HOSTSEND:
		case TEE_C_NONE:
			if (0 == waiting) {
				pr_info("%s: No message pending!\n", __FUNCTION__);
				waiting = 1;
			}
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
	const u8 number_of_pings = 1;
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

void attack_read_mem(void) {
	const u8 number_of_read_attempts = 5;
	u64 address = 0;
	u64 secret_mem_size = 4096;
	// We simply use the first byte of the payload to count the pings
	// Vector not yet initalized
	if (0 == SHARED_MEM_PTR->memory[0]) { return; }
	// The memory was initialized, we read
	if (number_of_read_attempts > SHARED_MEM_PTR->memory[1]) {
		void __iomem * secret_mem = NULL;
		// The 8 bytes beginning at memory SHARED_MEM_PTR->memory[2] denote a 
		// 64 bit physical memory addresss 
		address = *((u64*) &(SHARED_MEM_PTR->memory[2]));
		pr_info("Test 0x%016llx", address);
		secret_mem = ioremap(address, secret_mem_size);
		for (u64 offset = 0; offset < secret_mem_size; offset+=4) {
			ioread32(secret_mem + offset);
		}
		++SHARED_MEM_PTR->memory[1];
		SHARED_MEM_PTR->task_id = TEE_T_ATTACK_READ_MEM;
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached ping count %u", SHARED_MEM_PTR->memory[0]);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}

void attack_write_mem(void) {
	const u8 number_of_read_attempts = 5;
	u64 address = 0;
	u64 secret_mem_size = 4096;
	// We simply use the first byte of the payload to count the pings
	// Vector not yet initalized
	if (0 == SHARED_MEM_PTR->memory[0]) { return; }
	// The memory was initialized, we read
	if (number_of_read_attempts > SHARED_MEM_PTR->memory[1]) {
		void __iomem * secret_mem = NULL;
		// The 8 bytes beginning at memory SHARED_MEM_PTR->memory[2] denote a 
		// 64 bit physical memory addresss 
		address = *((u64*) &(SHARED_MEM_PTR->memory[2]));
		pr_info("Test 0x%016llx", address);
		secret_mem = ioremap(address, secret_mem_size);
		for (u64 offset = 0; offset < secret_mem_size; offset+=4) {
			iowrite32(SHARED_MEM_PTR->memory[1] + offset, secret_mem + offset);
			ioread32(secret_mem + offset);
		}
		++SHARED_MEM_PTR->memory[1];
		SHARED_MEM_PTR->task_id = TEE_T_ATTACK_WRITE_MEM;
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached ping count %u", SHARED_MEM_PTR->memory[0]);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}