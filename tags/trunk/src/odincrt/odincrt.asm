; $Id: odincrt.asm,v 1.4 2002-09-19 09:38:27 sandervl Exp $

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

IFDEF DEBUG
                EXTRN _delete_free:NEAR
                EXTRN _new_alloc:NEAR
ENDIF
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

	PUBLIC os2__nw__FUiPCcT1
	EXTERN __nw__FUiPCcT1:NEAR
;input: eax = size
;       ecx = line nr
;       edx = source filename
os2__nw__FUiPCcT1 proc near
	push	fs
	push	eax
	mov	ax, 150bh
	mov	fs, ax
	pop	eax
IFDEF DEBUG
        pushad
        push    eax
        call    _new_alloc
        add     esp, 4
        popad
ENDIF
	sub	esp, 0Ch
	call	__nw__FUiPCcT1
	add	esp, 0Ch	
	pop	fs
	ret
os2__nw__FUiPCcT1 endp

	PUBLIC os2__dl__FPvPCcUi
	EXTERN __dl__FPvPCcUi:NEAR
;input: eax = this ptr
;       ecx = line nr
;       edx = source filename
os2__dl__FPvPCcUi proc near
	push	fs
	push	eax
	mov	ax, 150bh
	mov	fs, ax
	pop	eax
IFDEF DEBUG
        pushad
        push    eax
        call    _delete_free
        add     esp, 4
        popad
ENDIF
	sub	esp, 0Ch
	call	__dl__FPvPCcUi
	add	esp, 0Ch	
	pop	fs
	ret
os2__dl__FPvPCcUi endp

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
