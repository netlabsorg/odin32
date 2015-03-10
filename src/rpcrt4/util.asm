; $Id: util.asm,v 1.1 2000-04-02 22:02:58 davidr Exp $
;
; /*
;  * Project Odin Software License can be found in LICENSE.TXT
;  * Win32 Exception handling + misc functions for OS/2
;  *
;  * Copyright 2000 Edgar Buerkle
;  *
;  */


.386p
		NAME	ole32util

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

public	_getEAX
_getEAX	proc	near
	ret
_getEAX endp

public	_getEDX
_getEDX	proc	near
	mov	EAX, EDX
	ret
_getEDX endp

CODE32          ENDS

END
