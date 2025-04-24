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
	TEE_T_ATTACK_WRITE_MEM = 0x03,
	TEE_T_ATTACK_NOP_MEM = 0x04,
};

struct shared_mem {
	u8 status;
	u8 task_id;
	u8 memory[];
};

int init_mgmt_module(u8 *shared_mem_ptr, int apic_id);
void cleanup_tee_poll_timer(void);
void write_to_com_mem(void* buffer, size_t len);
void ipi_attack(void);