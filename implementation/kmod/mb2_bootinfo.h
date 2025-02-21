#pragma once

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

struct mb_header {
	u32 magic;
	u32 arch;
	u32 hdr_len;
	u32 checksum;
};

struct mb2_bi_fixed_part
{
	u32 total_size;
	u32 reserved;
};

struct mb2_memory_map_entry
{
	u64 base_addr;
	u64 length;
	u32 type;
	u32 reserved;
};

struct mb2_memory_map 
{
	u32 type;
	u32 size;
	u32 entry_size;
	u32 entry_version;
};

// Management structures
struct mb2_mmap_entry
{
	struct mb2_memory_map_entry entry;
	struct mb2_mmap_entry *next;
};

struct mb2_mmap
{
	struct mb2_memory_map tag;
	struct mb2_mmap_entry *entries;
	struct mb2_mmap_entry *last_entry;
};

// Creates a memory map entry from given base address and the len of the region
// with the given type.
struct mb2_mmap_entry* create_mb2_mmap_entry(u64 base_addr, u64 len, u32 type);
void destroy_mb2_mmap_entry(struct mb2_mmap_entry* entry);
struct mb2_mmap* create_mb2_mmap(void);
// Destroy and free all memory acquired by the memory map and it's entries.
void destroy_mb2_mmap(struct mb2_mmap* mmap);
int mb2_mmap_add_entry(struct mb2_mmap* mmap, struct mb2_mmap_entry *entry);
// Returns the physical address of multiboot 2 boot information created from
// the given memory map.
u64 create_mb2_boot_info(struct mb2_mmap *mmap);



