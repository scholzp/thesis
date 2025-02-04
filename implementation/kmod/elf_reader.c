#include <linux/slab.h>	/* Needed for KERN_INFO */
#include <linux/gfp.h>
#include <linux/highmem.h>
#include "elf_reader.h"

MODULE_LICENSE("GPL");

void reader_hello(void) {
    pr_info("Hi from elf reader\n");
}

// Takes ownership of data
elf32_file_t* parse_elf32(u8 *data) {
    elf32_file_t *result = NULL;
    u8 *table_offset = 0;
    unsigned int table_size = 0;
    elf32_header_t *header = (elf32_header_t*) data;
    pr_info("Header %p, Magic = %d\n", header, header->ei_mag);

    unsigned long ph_impl_size = sizeof(elf32_program_header_t);
    unsigned long sh_impl_size = sizeof(elf32_section_header_t);

    if (NULL == data) {
        return NULL;
    }
    pr_info("1. Header magic: %x\n", header->ei_mag);
    // Check header magic
    if (header->ei_mag != elf_magic) {
        pr_info("Header magic not right! Found %x, expected %x\n", header->ei_mag, elf_magic);
        return NULL;
    }
    pr_info("2. Prog header size: %x; impl size: %lx\n", header->e_phentsize, ph_impl_size);
    if (header->e_phentsize != ph_impl_size) {
        return NULL;
    }
    pr_info("3.\n");
    if (header->e_shentsize != sh_impl_size) {
        return NULL;
    }
    pr_info("4.\n");
    result = kzalloc(sizeof(*result), GFP_KERNEL);
    result->header = header;
    
    //Parse program table and intialize pointers
    table_size = header->e_phnum;
    pr_info("Found %u program header.\n", table_size);
    if (table_size > 0) {
        u32 min_start = -1u;
        u32 max_end = 0u;
        e32_program_table_entry_t *e_current = NULL; 
        table_offset = (data + header->e_phoff);
        result->program_header_table = kzalloc(ph_impl_size, GFP_KERNEL);
        result->program_header_table->header = (elf32_program_header_t*) table_offset;
        result->program_header_table->next = NULL;
        e_current = result->program_header_table;
        min_start = e_current->header->p_paddr;
        max_end = e_current->header->p_paddr + e_current->header->p_filesz;
        for (unsigned int table_index = 1; table_index < table_size; ++table_index) {
            e32_program_table_entry_t *e_new = kzalloc(sizeof(*e_new), GFP_KERNEL);
            table_offset += ph_impl_size; 
            e_new->header = (elf32_program_header_t*) table_offset;
            if (PT_LOAD == e_new->header->p_type) {
                if (e_new->header->p_paddr < min_start) {
                    min_start = e_new->header->p_paddr;
                }
                if ((e_new->header->p_paddr + e_new->header->p_filesz) > max_end) {
                    max_end = e_new->header->p_paddr + e_new->header->p_filesz;
                }
            }
            e_new->next = NULL;
            e_current->next = e_new;
            e_current = e_current->next;
        }
        if (max_end > min_start) {
            result->loadable_segments_size = max_end - min_start;
            result->relocate_offset = min_start;
            pr_info("Min start %x, Reloc entry: %x\n", min_start, result->header->e_entry - min_start);
        }
    } else {
        result->program_header_table = NULL;
    }

    //Parse section table and intialize pointers
    table_size = header->e_shnum;
    if (table_size > 0) {
        e32_section_table_entry_t *e_current = NULL; 
        table_offset = (data + header->e_shoff);
        result->section_header_table = kzalloc(sh_impl_size, GFP_KERNEL);
        result->section_header_table->header = (elf32_section_header_t*) table_offset;
        result->section_header_table->next = NULL;
        e_current = result->section_header_table;
        for (unsigned int table_index = 1; table_index < table_size; ++table_index) {
            e32_section_table_entry_t *e_new = kzalloc(sizeof(*e_new), GFP_KERNEL);
            table_offset += sh_impl_size; 
            e_new->header = (elf32_section_header_t*) table_offset;
            e_new->next = NULL;
            e_current->next = e_new;
            e_current = e_current->next;
        }
    } else {
        result->section_header_table = NULL;
    }
    result->data = data;
    return result;
}

// Loads all loadable segement to memory and returns address of entry point
// relocated to the address
u64 load_elf32_segments(elf32_file_t *elf) {
    // u8 *result = NULL;
    // e32_program_table_entry_t *entry = elf->program_header_table;
    // Code that prints "Ready for elf" over serial port 0xf38
    unsigned char test_msg[] = {
        0x66, 0xba, 0xf8, 0x03, 0x66, 0xb8, 0x52, 0x00, 0xee, 0x66, 
        0xb8, 0x65, 0x00, 0xee, 0x66, 0xb8, 0x61, 0x00, 0xee, 0x66, 
        0xb8, 0x64, 0x00, 0xee, 0x66, 0xb8, 0x79, 0x00, 0xee, 0x66, 
        0xb8, 0x20, 0x00, 0xee, 0x66, 0xb8, 0x66, 0x00, 0xee, 0x66,
        0xb8, 0x6f, 0x00, 0xee, 0x66, 0xb8, 0x72, 0x00, 0xee, 0x66,
        0xb8, 0x20, 0x00, 0xee, 0x66, 0xb8, 0x65, 0x00, 0xee, 0x66,
        0xb8, 0x6c, 0x00, 0xee, 0x66, 0xb8, 0x66, 0x00, 0xee, 0x66,
        0xb8, 0x0a, 0x00, 0xee, 0xf4, 0x0f, 0x0b
    }; 
    unsigned int test_msg_len = 77;

    //TODO: Check if we can replace magic 4096 by PAGE_SIZE
    // Calculate how many pages we require
    unsigned pages_order = 0;
    //size_t new_entry = elf->header->e_entry - elf->relocate_offset;
    for (size_t mem_to_allocate = 4096; mem_to_allocate < elf->loadable_segments_size; mem_to_allocate = mem_to_allocate << 1){
        ++pages_order;
    }
    pr_info("Allocate 2^%u pages \n", pages_order);
    struct page *section_mem = alloc_pages(GFP_KERNEL, pages_order);
    // map the section memory so we can write to it
    u8 *target = kmap(section_mem);
    // while (NULL != entry) {
    //     elf32_program_header_t *header = entry->header;
    //     if (PT_LOAD == header->p_type) {
    //         memcpy((target + header->p_paddr - elf->relocate_offset), (elf->data + header->p_offset), header->p_filesz);
    //     }
    //     entry = entry->next;
    // }
    for (u16 x = 0; x < test_msg_len; ++x) {
        target[x] = test_msg[x];
    }
    kunmap(section_mem);
    pr_info("Entry from ELF: %x\n", elf->header->e_entry);
    // pr_info("Reseved PFN %lx, Address start: %lx", page_to_pfn(section_mem), page_to_pfn(section_mem) * 4096 + new_entry);
    return (page_to_pfn(section_mem) * 4096 );//new_entry);
};