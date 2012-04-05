; loader.s

; standalone bootloader : gets into protected 
; mode and simply reads the partition table
; then, chainloads into stage1, on the bootloader of 
; the first partition

[BITS 16]
[ORG 0x7C00]

jmp start

%include "../include/asm/bootutil.s"
diskerror:
    mov si, diskerrormsg
    call println
    .loop:
        hlt
        jmp .loop

start:

mov ax, cs
mov ds, ax
mov es, ax

mov [bootdrive], dl   

mov ah, 0x0
mov al, 0x3
int 0x10

; Hello !
mov si, bootmsg
call println

xor ax, ax
int 0x13

push es
mov ax, 0x0
mov es, ax
mov bx, 0x7E00
mov al, 0x1
mov ah, 0x2
mov cl, 0x2
mov ch, 0x0
mov dl, [bootdrive]
mov dh, 0x0
int 0x13
jc diskerror
pop es

xor cx, cx

; browse partition table and boot first DOS partition with boot flag
.for:

    mov bx, cx
    shl bx, 0x4
    add bx, 0x7DBE

    ; bootable partition ?
    cmp byte [bx], 0x80
    jne .next

    ; DOS partition ?
    cmp byte [bx+0x4], 0x0C
    jne .next

    ; load the first sector of partition
    pusha

    mov ax, 0x1000
    mov es, ax

    ; use CHS adress
    mov ah, 0x02
    mov dl, [cs:bootdrive]
    mov dh, [bx+0x01]
    mov cl, [bx+0x02]
    mov ch, [bx+0x03]
    mov al, 1
    mov bx, 0x0000
    int 0x13
    jc diskerror
    popa

    ; jump into FAT boot code
    mov dl, [cs:bootdrive]
    jmp 0x1000:0x0000

    ; next partition
    .next:

    inc cx
    cmp cx, 4
    jne .for

    stop:
    jmp stop

bootdrive: db 0
bootmsg: db "Quasar 0.1 bootloader", 13, 10, 0
diskerrormsg: db "FATAL: DISK FAILURE", 13, 10, 0

times 446-($-$$) db 144