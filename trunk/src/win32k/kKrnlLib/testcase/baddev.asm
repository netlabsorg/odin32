; $Id: baddev.asm,v 1.2 2002-01-08 23:25:59 bird Exp $
;
; DevFirst - entrypoint and segment definitions
;
; Copyright (c) 2001 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Include files
;
    include devsegdf.inc

CODE16 segment
    ASSUME CS:CODE16, DS:DATA16, ES:NOTHING, SS:NOTHING

CODE16START db 'CODE16START',0
    .286p
;;
; Stub which pushes parameters onto the stack and call the 16-bit C strategy routine.
; @returns   returns the return value of strategy(...)
; @author    knut st. osmundsen
_strategyAsm proc far
	cmp     byte ptr es:[bx+2], 0       ; CMDInit
    je      init
	cmp     byte ptr es:[bx+2], 27      ; CMDInitBase
    je      init
    jmp     error

    ;
    ; Init request packet - fill answer and return successfully.
    ;
init:
    xor     ax, ax
	mov	    word ptr es:[bx+20], ax
	mov	    word ptr es:[bx+18], ax
	mov	    byte ptr es:[bx+13], al
    mov     word ptr es:[bx+3],  0100h
	mov	    word ptr es:[bx+14], offset CODE16_INITSTART
	mov	    word ptr es:[bx+16], offset DATA16_INITSTART
    retf

    ;
    ; Whatever other packet - fail.
    ;
error:
    mov     word ptr es:[bx+3], 8103h
    retf
_strategyAsm endp
    .386p
CODE16 ends

CODE16_INIT segment
CODE16_INITSTART db 'CODE16_INITSTART',0
CODE16_INIT ends

CODE32 segment
CODE32START db 'CODE32START',0
    db 010000h dup(90)                  ; FILLER
CODE32 ends

DATA16 segment
DATA16START label byte                  ; Note. no start string here!
_aDevHdrs:
    DD	DATA16_GROUP:_aDevHdrs+30
	DW	08980H
	DW	_strategyAsm
	DW	00H
	DB	'kKrnlHlp'
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DD	03H

	DD	0ffffffffH
	DW	08980H
	DW	_strategyAsm
	DW	00H
	DB	'kKrnlLib'
	DW	00H
	DW	00H
	DW	00H
	DW	00H
	DD	03H
DATA16 ends

DATA16_BSS segment
DATA16_BSSSTART db 'DATA16_BSSSTART',0
DATA16_BSS ends

DATA16_CONST segment
DATA16_CONSTSTART db 'DATA16_CONSTSTART',0
DATA16_CONST ends

DATA16_GLOBAL segment
DATA16_GLOBALSTART db 'DATA16_GLOBALSTART',0
_DATA16Base dd offset FLAT:DATA16:DATA16START
DATA16_GLOBAL ends

DATA16_INIT segment
DATA16_INITSTART db 'DATA16_INITSTART',0
DATA16_INIT ends

DATA32 segment
DATA32START db 'DATA32START',0
    db 10h dup(90)                      ; FILLER
DATA32 ends

_VFT segment
    db 0ch dup(90)
_VFT ends

BSS32 segment
BSS32START db 'BSS32START',0
BSS32 ends

CONST32_RO segment
CONST32_ROSTART db 'CONST32_ROSTART', 0
CONST32_RO ends

_VFT segment
_VFTSTART db '_VFTSTART', 0
_VFT ends

EH_DATA segment
EH_DATASTART db 'EH_DATASTART', 0
EH_DATA ends

CALLTAB16 segment para public 'CALLTAB16_DATA' use16
    db 0d6h dup(90)
CALLTAB16 ends

LOGDATA_16BIT segment word public 'LOGDATA_16BIT' use16
ulfpBuffers LABEL DWORD
usOffBuffers    dw  offset LOGDATA_16BIT:achBuffers
usSelBuffers    dw  seg    LOGDATA_16BIT:achBuffers
achBuffers      db  0fffch dup(0)
LOGDATA_16BIT ends

SYMBOLDB32 segment
    db 10h dup(90)
SYMBOLDB32 ends



END


