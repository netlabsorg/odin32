; $Id: odincrt.asm,v 1.2 1999-11-09 11:29:06 achimha Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 1998 Sander van Leeuwen
; *
; */

; 1999/08/09 PH see if we can do this as INLINE functions

.386p
                NAME    odinfs

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  DS:FLAT, SS:FLAT

                PUBLIC  __threadid 
__threadid proc near
	push    ebp
	push	fs
        mov     ax, 150bh
	mov	fs, ax
		
        mov     eax,dword ptr fs:[0000000cH]

	pop	fs
        mov     ebp,esp
        leave   
        ret     
__threadid endp

        PUBLIC GetFS
GetFS   proc near
        mov     eax, fs
        ret
GetFS   endp


        PUBLIC SetFS
SetFS   proc near
        mov     eax, [esp+4]
        mov     fs, eax
        ret
SetFS   endp


        PUBLIC SetReturnFS
SetReturnFS proc near
        push    fs
        mov     eax, [esp+8]
        mov     fs, eax
        pop     eax
        ret
SetReturnFS endp


        PUBLIC RestoreOS2FS
RestoreOS2FS proc near
        push    150bh           ; @@@PH that's NOT the clean way ! :)
        mov     ax, fs
        pop     fs
        ret
RestoreOS2FS endp

CODE32          ENDS
                END
