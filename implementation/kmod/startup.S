.code16
.text
    mov $0x3F8, %dx
    mov $0x65, %ax
    out %al, %dx 
    hlt
    