#include <asm/delay.h>
#include <asm/io.h>

#include "lapic.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

void __iomem * LAPIC_PAGE = NULL;
static char INITIALIZED = 0;

void lapic_init(void) 
{
	u64 lapic_addr;
	// Read the address from the APIC Base Address Register of the CPU
	rdmsrl(0x0000001b, lapic_addr);
	LAPIC_PAGE = ioremap(lapic_addr & (~0xFFFu), 4096);
	iowrite32(0 , LAPIC_PAGE + 0x280);
	INITIALIZED = 1;
}

void lapic_send_init_ipi_waiting(u32 target_lapic_id) {
	if (1 != INITIALIZED) 
	{
		pr_err("%s: LAPIC not initialized", __FUNCTION__);
		return;
	}
	iowrite32(
		(ioread32(LAPIC_PAGE + 0x310) & 0x00ffffff) | (target_lapic_id << 24),
		LAPIC_PAGE + 0x310
	);
	iowrite32(
		(ioread32(LAPIC_PAGE + 0x300) & 0xfff00000) | 0x00C500,
		LAPIC_PAGE + 0x300
	);
	do {
		__asm__ __volatile__ ("pause" : : : "memory"); 
	} while (ioread32(LAPIC_PAGE + 0x300) & (1 << 12));
}

void lapic_send_startup_ipi_waiting(u32 target_lapic_id, char segment) {
	if (1 != INITIALIZED) 
	{
		pr_err("%s: LAPIC not initialized", __FUNCTION__);
		return;
	}
	iowrite32(0, LAPIC_PAGE + 0x280); 
	iowrite32(
		(ioread32(LAPIC_PAGE + 0x310) & 0x00ffffff) | (target_lapic_id << 24),
		LAPIC_PAGE + 0x310
	);
	// set delivery mode to 0x6 (startup) and vector to 0xc8 (segment of reserved low mem)
	iowrite32(
		(ioread32(LAPIC_PAGE + 0x300) & 0xfff0f800) | 0x000600 | segment,
		LAPIC_PAGE + 0x300
	);
	udelay(200);
	do {
		__asm__ __volatile__ ("pause" : : : "memory");
	} while (ioread32(LAPIC_PAGE + 0x300) & (1 << 12));
}

void release_lapic(void) {
	iounmap(LAPIC_PAGE);
}