.code16
.text
    mov $0x3f8, %dx #Load port address 0x3f8
    mov $0x31, %ax  #Print '1337 AP ALIVE' through port
    out %al, %dx 
    mov $0x33, %ax
    out %al, %dx
    mov $0x33, %ax
    out %al, %dx
    mov $0x37, %ax
    out %al, %dx
    mov $0x20, %ax
    out %al, %dx 
    mov $0x41, %ax
    out %al, %dx
    mov $0x50, %ax
    out %al, %dx
    mov $0x20, %ax
    out %al, %dx
    mov $0x41, %ax
    out %al, %dx
    mov $0x4c, %ax
    out %al, %dx
    mov $0x49, %ax
    out %al, %dx
    mov $0x56, %ax
    out %al, %dx
    mov $0x45, %ax
    out %al, %dx
    mov $0x0a, %ax
    out %al, %dx
    hlt
