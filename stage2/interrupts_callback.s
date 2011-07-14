extern c_callback, c_callback_kbd
global _asm_callback, _asm_callback_kbd

_asm_callback:
	call c_callback
	mov al,0x20
	out 0x20,al
	iret

_asm_callback_kbd:
    call c_callback_kbd
    mov al,0x20
    out 0x20,al
    iret