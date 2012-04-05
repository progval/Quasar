[BITS 16]

; stage1 main entry
; this is just a little code snippet, as it must reside on the FAT32 partition
; VBR, which is limited to 420 bytes for the boot code section.
; this is just a chainloader that will load the reserved sectors of the FAT
; parition where the main stage1 code is.

jmp start

%include "../include/asm/bootutil.s"
diskerror:
    mov si, diskerrormsg+0x3E
    call println
    .loop:
        hlt
        jmp .loop

start:

    mov ax, cs
    mov ds, ax

    ; es will be used to address the bootloader
    ; data segment where he stored its values
    mov ax, 0x0000
    mov es, ax

    mov si, msg+0x3E
    call println

    ; the VBR sector tells us the count 
    ; and location of the reserved sectors
    ; it has already been loaded into memory 
    ; by the bootloader at the current segment
    ; so all one has to do is read the correct
    ; values. This value is coded on bytes 14
    ; and 15 of the VBR, in litte endian

    ; read upper bytes and shifts them
    xor ax, ax
    mov ax, [cs:15]
    shl ax, 8

    ; add lower bytes
    or ax, [cs:0xE]

    ; the VBR is included in reserved sectors
    ; so count only the n-1 reserved sectors
    dec ax

    ; the bootloader must have stored in bx 
    ; the address where the partition table
    ; has been read, and in dl the bootdrive
    mov ah, 0x2
    mov cl, [es:bx+2]
    mov ch, [es:bx+3]
    mov dh, [es:bx+1]
    inc cl

    ; and load them an 0x2000:0x0000
    mov bx, 0x0000
    push ax
    mov ax, 0x2000
    mov es, ax
    pop ax

    int 0x13
    jc diskerror

    jmp 0x2000:0x0000

end:
    jmp end

msg: db "Quasar stage1 running...", 0
diskerrormsg: db "FATAL: DISK FAILURE", 13, 10, 0

