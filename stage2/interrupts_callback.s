extern c_callback_kbd, c_callback_pagefault
global _asm_callback, _asm_callback_kbd, _asm_callback_pagefault

_asm_callback:
	mov al,0x20
	out 0x20,al
	iret

_asm_callback_kbd:
    call c_callback_kbd
    mov al,0x20
    out 0x20,al
    iret

_asm_callback_pagefault:
    call c_callback_pagefault
    mov al,0x20
    out 0x20,al
    iret