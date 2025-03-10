#pragma once
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

enum tee_command{
	TEE_C_NONE = 0x00,
	TEE_C_TEE_READY = 0x01,
	TEE_C_TEE_SEND = 0x02,
	TEE_C_HOSTSEND = 0x11,
};

enum tee_task{
	TEE_T_UNKNOWN = 0xff,
	TEE_T_PING = 0x01,
	TEE_T_ATTACK_READ_MEM = 0x02,
};

struct shared_mem {
	u8 status;
	u8 task_id;
	u8 memory[];
};

int setup_tee_poll_timer(u8 *shared_mem_ptr);
void cleanup_tee_poll_timer(void);