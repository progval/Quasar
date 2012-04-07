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

no_long_mode:
    mov si, msgnolongmode+0x3E
    call println
    .loop:
        hlt
        jmp .loop

msgnolongmode: db "ERROR: Quasar can't run on 32 bits CPU", 0

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

    ; and load them an 0x20000:0x0000
    mov bx, 0x0000
    push ax
    mov ax, 0x2000
    mov es, ax
    pop ax

    int 0x13
    jc diskerror

    ; check long mode support 
    mov eax, 0x80000000
    cpuid
    cmp   eax, 0x80000000
    jbe   near no_long_mode
    mov   eax, 0x80000001
    cpuid                
    bt    edx, 29        
    jnc   no_long_mode 

    lgdt [0x10108]

    jmp 0x8:0x1F

    end:
        jmp end


pGDT32:                         ; Used by LGDT.
    dw 5 * 8                ; GDT limit ...
    dd 0x10110     ; and 32-bit GDT base
    dw 0

GDT32:
        SegNull:
                dw 0x0000               ; Limite 0-15
                dw 0x0000               ; Base Addr 0-15
                db 0x00                 ; Base Addr 16-23
                dw 0000000000000000b    ; Flags
                db 0x00                 ; Base Addr 24-31
        Code32: ; Code 32 bits qui commence à 0x20000
                dw 0xFFFF               ; Limite 0-15
                dw 0x0000               ; Base Addr 0-15
                db 0x02                 ; Base Addr 16-23
                dw 1100111110011110b    ; Flags
                db 0x00                 ; Base Addr 24-31
        mStack: ; Pile
                dw 0xFFFF               ; Limite 0-15
                dw 0x0000               ; Base Addr 0-15 
                db 0x00                 ; Base Addr 16-23
                dw 1000000010010010b    ; Flags
                db 0x00                 ; Base Addr 24-31
        Data32: ; Données du Stage2 en mode protégé
                dw 0xFFFF               ; Limite 0-15
                dw 0x0000               ; Base Addr 0-15
                db 0x02                 ; Base Addr 16-23
                dw 1000111110010010b    ; Flags
                db 0x00                 ; Base Addr 24-31
        General: ; Segment général
                dw 0xFFFF               ; Limite 0-15
                dw 0x0000               ; Base Addr 0-15 
                db 0x00                 ; Base Addr 16-23
                dw 1000111110010010b    ; Flags
                db 0x00                 ; Base Addr 24-31

msg: db "Quasar stage1 running...", 0
diskerrormsg: db "FATAL: DISK FAILURE", 13, 10, 0

