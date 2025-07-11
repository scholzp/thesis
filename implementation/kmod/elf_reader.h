#pragma once
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pascal Scholz <pascal.scholz@cyberus-technology.de>");

#define elf_magic 0x464c457fu

typedef struct elf32_header {
	u32 ei_mag;
	u8 ei_class;
	u8 ei_data;
	u8 ei_version;
	u8 ei_osabi;
	u8 ei_abiversion;
	u8 ei_pad[7];
	u16 e_type;
	u16 e_machine;
	u32 e_version;
	u32 e_entry;
	u32 e_phoff;
	u32 e_shoff;
	u32 e_flags;
	u16 e_ehsize;
	u16 e_phentsize;
	u16 e_phnum;
	u16 e_shentsize;
	u16 e_shnum;
	u16 e_shstrndx;
} elf32_header_t;

typedef struct elf32_program_header {
	u32 p_type;
	u32 p_offset;
	u32 p_vaddr;
	u32 p_paddr;
	u32 p_filesz;
	u32 p_memsz;
	u32 p_pflags;
	u32 p_align;
}elf32_program_header_t;

typedef struct elf32_section_header {
	u32 sh_name;
	u32 sh_type;
	u32 sh_flags;
	u32 sh_paddr;
	u32 sh_offset;
	u32 sh_size;
	u32 sh_link;
	u32 sh_info;
	u32 sh_addralign;
	u32 sh_entsize;
}elf32_section_header_t;

typedef struct e32_program_table_entry {
	elf32_program_header_t *header;
	struct e32_program_table_entry *next;
} e32_program_table_entry_t;

typedef struct e32_section_table_entry {
	elf32_section_header_t *header;
	struct e32_section_table_entry *next;
} e32_section_table_entry_t;

typedef struct elf32_file {
	elf32_header_t *header;
	e32_program_table_entry_t *program_header_table;
	e32_section_table_entry_t *section_header_table;
	u8* data;
	size_t loadable_segments_size;
	size_t relocate_offset;
} elf32_file_t;

elf32_file_t* parse_elf32(u8 *data);
u64 load_elf32_segments(elf32_file_t *elf);
size_t align_up (size_t value, size_t align_base);
