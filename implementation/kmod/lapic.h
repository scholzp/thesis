#pragma once
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

extern void __iomem * LAPIC_PAGE;

// Init the LAPIC module. This maps the LAPIC page to memory. Needs to be 
// called once. Call release_lapic() when done to free resources. 
void lapic_init(void);
// Sends an INIT IPI to the CPU with the given LAPIC ID.
void lapic_send_init_ipi_waiting(u32 target_lapic_id);
// Sends an STARTUP IPI to the CPU with the given LAPIC ID. Currently sends IPI
// with fixed segment address. FIXME: We should change this
void lapic_send_startup_ipi_waiting(u32 target_lapic_id);
// Release the LAPIC by unmapping the the page
void release_lapic(void);