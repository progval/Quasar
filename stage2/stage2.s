; stage2.s
;  
; Copyright (C) 2011 Leo Testard <leo.testard@gmail.com>
;                    Florent Revest <florent.revest666@gmail.com>
;  
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

; main entry point in ASM for stage2

gdt:
    null_desc:
        dd 0, 0
    code_desc:
        dw 0x0FFFF
        dw 0
        dw 0x9A00
        dw 0x00C0
    data_desc:
        dw 0x0FFFF
        dw 0
        dw 0x9200
        dw 0x00C0
gdt_end:

gdt_desc:
    dw gdt_end - gdt -1
    dd gdt

clear_screen:
    push eax
    push ebx
    mov ebx, 0xB8000
    mov eax, 4000
    .clear_screen_start:
        cmp eax,0
        jz .clear_screen_end
        inc ebx
        mov byte [ebx], 0
        dec eax
        jmp .clear_screen_start
    .clear_screen_end:
        pop ebx
        pop eax
        ret

[GLOBAL start] ; start is our global entry point
[EXTERN main] ; make the stage2's C main accessible from this code

start:
    pusha
    lgdt [gdt_desc]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    jmp 0x08:next
    next:
    popa
    call clear_screen
    push ebx     ; the multi boot structure
    push eax     ; the mutliboot bootloader magic number
    call main    ; enter C code
    jmp $        ; infinite loop
