.code16
.text
bin_begin:
    // NOP slide. The CPU will later override this, when it enters the boot 
    // code at the beginning of the segment. That's ok, as long as no second 
    // CPU comes along 
stack:
    .long 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090
    .long 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090
    .long 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090
    .long 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090
    .long 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090
    .long 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090, 0x90909090
start:
    // Setup data segment (We just use this page as data segment)
    mov $0xc800, %ax
    mov %ax, %ds
    // Setup stack, last 1Kib of the page
    mov $start, %bx
    sub $stack, %bx
    mov %ax, %ss
    mov %bx, %sp

    // Print hello
    mov $hello_txt, %bx
    mov $ready_txt, %cx
    sub %bx, %cx
    sub $bin_begin, %bx
    mov $0x3f8, %dx #Load port address 0x3f8
print16:
    mov (%bx), %ax
    out %al, %dx
    sub $1, %cx
    add $1, %bx
    cmp $0, %cx
    jne print16
enter_protected:
    cli
    mov $gdtdesc, %ebx
    sub $bin_begin, %ebx
    lgdt (%ebx)
    //lgdt %cs:gdtdesc
    mov %cr0, %eax
    mov $gdt, %ecx
    or $0x1, %al
    mov %eax, %cr0
    mov $protected, %edx
    //sub %bin_begin, %edx
    hlt
    //ljmp $0x8, $protected

.code32
protected:
    /* TODO: Insert long jump here. */
    // print the ready string, we are still alive
    hlt
    mov $ready_txt, %bx
    mov $ready_txt_end, %cx
    sub %bx, %cx
    sub $bin_begin, %bx
    mov $0x3f8, %dx #Load port address 0x3f8
print32:
    mov (%bx), %ax
    out %al, %dx
    sub $1, %cx
    add $1, %bx
    cmp $0, %cx
    jne print32
    // Stop execution once we skip the hlt
    hlt
    ud2

// Print function
// Calling convention: bx=address of string, cx=length of string
   # sti

.p2align	5	/* force 32-byte alignment */
gdt:
	.word	0, 0
	.byte	0, 0, 0, 0
	/* -- code segment --
	 * base = 0x00000000, limit = 0xFFFFF (4 KiB Granularity), present
	 * type = 32bit code execute/read, DPL = 0
	 */
	.word	0xFFFF, 0
	.byte	0, 0x9A, 0xCF, 0
	/* -- data segment --
	 * base = 0x00000000, limit 0xFFFFF (4 KiB Granularity), present
	 * type = 32 bit data read/write, DPL = 0
	 */
	.word	0xFFFF, 0
	.byte	0, 0x92, 0xCF, 0
//this is the GDT descriptor
gdtdesc:
	.word	0x17	/* limit */
	.long	gdt		/* addr */
	.p2align	2
hello_txt:
    .ascii "o7 AP ALIVE\n"
ready_txt:
    .ascii "Ready for long jump\n"
ready_txt_end:
long_jump_off:
    .long 0x01020304
    nop
