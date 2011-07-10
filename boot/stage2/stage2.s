; stage2.s
; main entry point in ASM for stage2
; gets the structure given by the multiboot bootloader and calls the
; main C method

; multiboot compliance
MBOOT_PAGE_ALIGN      equ 1<<0
MBOOT_MEM_INFO        equ 1<<1
MBOOT_VESA            equ 1<<2
MBOOT_HEADER_MAGIC    equ 0x1BADB002 ; multiboot magic value
MBOOT_HEADER_FLAGS    equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_VESA
MBOOT_CHECKSUM        equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

; VESA
VESA_MODE             equ 0
VESA_WIDTH            equ 800
VESA_HEIGHT           equ 600
VESA_DEPTH            equ 32

; As the multiboot bootloader as already switched to protected mode,
; we can use 32 bits instructions
[BITS 32]

[GLOBAL mboot]
[EXTERN code]
[EXTERN bss]
[EXTERN end]

mboot:
    dd    MBOOT_HEADER_MAGIC
    dd    MBOOT_HEADER_FLAGS
    dd    MBOOT_CHECKSUM
    dd    mboot
    dd    code
    dd    bss
    dd    end
    dd    start

; start is our global entry point
[GLOBAL start]

; make the stage2's C main accessible from this code
[EXTERN main]

start:
    push ebx     ; the multi boot structure
    push eax     ; the mutliboot bootloader magic number
    cli          ; shut down interrupts
    call main    ; enter C code
    jmp $        ; infinite loop
