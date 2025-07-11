#include <linux/slab.h>	/* Needed for KERN_INFO */
#include <linux/gfp.h>
#include <linux/highmem.h>

#include "mb2_bootinfo.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");


// Creates a memory map entry from given base address and the len of the region
// with the given type.
struct mb2_mmap_entry* create_mb2_mmap_entry (u64 base_addr, u64 len, u32 type)
{
	struct mb2_mmap_entry* entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	pr_info("%s : called\n", __FUNCTION__);
	pr_info("%s : Allocated memory at %p\n", __FUNCTION__, entry);
	entry->entry.base_addr = base_addr;
	entry->entry.length = len;
	entry->entry.type = type;
	entry->next = NULL;
	return entry;
}

void destroy_mb2_mmap_entry(struct mb2_mmap_entry* entry) 
{
	kfree(entry);
}

struct mb2_mmap* create_mb2_mmap (void)
{
	struct mb2_mmap *mmap = kzalloc(sizeof(*mmap), GFP_KERNEL);
	pr_info("%s : Allocated memory at %p\n", __FUNCTION__, mmap);
	mmap->entries = NULL;
	mmap->tag.type = 0x6;
	mmap->tag.size = sizeof(mmap->tag);
	mmap->tag.entry_size = sizeof(struct mb2_memory_map_entry);
	return mmap;
}

// Destroy and free all memory acquired by the memory map and it's entries.
void destroy_mb2_mmap(struct mb2_mmap* mmap) 
{
	struct mb2_mmap_entry *entry = mmap->entries;
	while (NULL != entry)
	{
		struct mb2_mmap_entry *next = entry->next;
		kfree(entry);
		entry = next;
	}
	kfree(mmap);
}


int mb2_mmap_add_entry (struct mb2_mmap *mmap, struct mb2_mmap_entry *entry)
{
	if (NULL == mmap) 
	{
		pr_info("%s :Given mmap is Null pointer!\n", __FUNCTION__);
	}
	if (NULL == entry) 
	{
	 	pr_info("%s :Given entry is Null pointer!\n", __FUNCTION__);
	}	
	// init first entry of mmap
	if (NULL == mmap->entries) {
		mmap->entries = entry;
	} else {
		mmap->last_entry->next = entry;
	}
	mmap->last_entry = entry;
	mmap->tag.size += sizeof(entry->entry);
	return 0;
}

// Returns the physical address of multiboot 2 boot information created from
// the given memory map.
u64 create_mb2_boot_info(struct mb2_mmap *mmap) 
{
	// TODO: FIXME: This function lead to memory leaking. We don't store the 
	// result of alloc_pages in a way that we can free them later.
	u8* virt_addr;
	// total size in bytes
	u32 total_size = 0;
	u64 mbi_mem_offset = 0;
	u64 to_allocate = 4096;
	u32 pages_order = 1;
	struct page *mbi_mem;
	struct mb2_bi_fixed_part terminating_tag = {0, 8};
	struct mb2_bi_fixed_part fixed_part;

	if (NULL == mmap)
	{
		pr_info("%s :Given mmap is Null pointer!", __FUNCTION__);
		return 0;
	}

	// Calculate the size in bytes from the individual parts
	// Fixed part tag
	total_size += sizeof(fixed_part);
	//Size of memory map tag
	total_size += mmap->tag.size;
	// Terminating tag
	total_size += sizeof(terminating_tag);
	fixed_part.total_size = total_size;

	// allocate enough memory for the
	while (to_allocate < total_size)
	{
		to_allocate *= 4096;
		++pages_order; 
	}
	mbi_mem = alloc_pages(GFP_KERNEL, pages_order);
	virt_addr = kmap(mbi_mem);
	
	// Copy data form the respective structures to the mbi memory
	// Copy fixed part
	memcpy(virt_addr + mbi_mem_offset, &fixed_part, sizeof(fixed_part));
	mbi_mem_offset += sizeof(fixed_part);
	// Copy all tags, currently only memory map
	memcpy(virt_addr + mbi_mem_offset, &(mmap->tag), sizeof(mmap->tag));
	mbi_mem_offset += sizeof(mmap->tag);
	{
		// Copy each memory map entry
		struct mb2_mmap_entry* current_mmap_entry = mmap->entries;
		while (NULL != current_mmap_entry) {
			pr_info("Copy entry to offset %llu\n", mbi_mem_offset);
			memcpy(
				virt_addr + mbi_mem_offset,
				&(current_mmap_entry->entry),
				sizeof(current_mmap_entry->entry)
			);
			mbi_mem_offset += sizeof(current_mmap_entry->entry);
			current_mmap_entry = current_mmap_entry->next;
		}
	}
	// Copy data of end tag
	memcpy(
		virt_addr + mbi_mem_offset,
		&terminating_tag,
		sizeof(terminating_tag)
	);
	kunmap(mbi_mem);
	// return the physical address
	return page_to_pfn(mbi_mem) * 4096;
}
