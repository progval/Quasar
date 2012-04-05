; convenience file that contains screen IO functions used
; by the parts of the boot process (bootloader, chainloader),
; and stage1

println:
    push ax 
    push bx
    call print
    mov ah, 0x0E
    mov bx, 0x07
    mov al, 0x0D
    int 0x10
    mov al, 0x0A
    int 0x10
    pop bx
    pop ax
    ret

print:
    push ax 
    push bx
    .start:
        lodsb
        cmp al, 0
        jz .end
        mov ah, 0x0E
        mov bx, 0x07
        int 0x10
        jmp .start
    .end:
    pop bx
    pop ax
    ret