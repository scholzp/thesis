.code16
.text
bin_begin:
    // NOP slide. The CPU will later override this, when it enters the boot 
    // code at the beginning of the segment. That's ok, as long as no second 
    // CPU comes along 
start:
    // Setup data segment (We just use this page as data segment)
    mov $0xc800, %ax
    mov %ax, %ds

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
    mov $protected, %edx
    movl %cr0, %eax
    mov $gdt, %ecx
    or $0x1, %al
    movl %eax, %cr0
    //sub %bin_begin, %edx
    //hlt
    ljmpl $0x8, $protected

.code32
protected:
    /* TODO: Insert long jump here. */
    // print the ready string, we are still alive
    movl $0x10, %eax
    movl %eax, %ds
    movl $ready_txt, %ebx
    movl $ready_txt_end, %edi
    // DEbug
    sub %ebx, %edi
    mov $0x3f8, %dx #Load port address 0x3f8
print32:
    // This differs from print16 because we now have flat CS and DS
    mov (%ebx), %eax
    out %al, %dx
    sub $1, %di
    add $1, %ebx
    cmp $0, %di
    jne print32
    // Flat segmentation; We can jump where ever we want. This time the actual 
    // kernel's address is our target. The address is stored in target_kernel_addr
    movl target_kernel_addr, %ecx 
    jmp %ecx
    // Stop execution once we skip the hlt
    hlt
    ud2
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
    .ascii "Ready to jump into kernel\n"
ready_txt_end:
target_kernel_addr:
    .long 0x01020304
