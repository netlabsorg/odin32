; $Id: devfirst.asm,v 1.1 1999-09-06 02:19:56 bird Exp $
;
; DevFirst - entrypoint and segment definitions
;
; Copyright (c) 1999 knut st. osmundsen
;
;
    .386p

;
; Include files
;
    include devsegdf.inc

;
; Exported symbols
;
    public CODE16START
    public DATA16START
    public DATA16START
    public DATA16_BSSSTART
    public DATA16_CONSTSTART
    public CODE16START
    public CODE32START
    public DATA32START
    public BSS32START
    public CONST32_ROSTART

    public _strategyAsm0
    public _strategyAsm1
    public _SSToDS_16a


;
; Externs
;
    extrn _strategy:near
    extrn CODE16END:byte
    extrn DATA16END:byte
    extrn _TKSSBase16:dword


CODE16 segment
    ASSUME CS:CODE16, DS:DATA16, ES:NOTHING, SS:NOTHING

CODE16START label byte

;$win32ki entry point
_strategyAsm0:
    push    0
    jmp     _strategyAsm

;$win32k entry point
_strategyAsm1:
    push    1
    jmp     _strategyAsm

_strategyAsm proc far
    push    es
    push    bx
    push    ds
    pop     es
    call    _strategy
    pop     bx
    pop     es
    mov     word ptr es:[bx+3], ax
    add     sp, 2
;    int 3
    retf
_strategyAsm endp


;extern LIN   SSToDS_16a(void NEAR *pStackVar);
_SSToDS_16a proc near
    assume CS:CODE16, DS:DATA16, ES:NOTHING
    push    es
    jmp far ptr FLAT:_SSToDS_16a_GetFLAT_32
_SSToDS_16a_GetFLAT_16::
    mov     edx, ds:_TKSSBase16
    movzx   eax, word ptr ss:[esp + 4]
    add     eax, es:[edx]
    mov     edx, eax
    shr     edx, 16
    pop     es
    ret
_SSToDS_16a endp

CODE16 ends

CODE32 segment
CODE32START label byte
    ASSUME CS:CODE32
_SSToDS_16a_GetFLAT_32:
    push    cs
    pop     es
    jmp far ptr CODE16:_SSToDS_16a_GetFLAT_16

CODE32 ends


;
; all segments have a <segmentname>START label at the start of the segment.
;
DATA16 segment
DATA16START label byte
DATA16 ends

DATA16_BSS segment
DATA16_BSSSTART label byte
DATA16_BSS ends

DATA16_CONST segment
DATA16_CONSTSTART label byte
DATA16_CONST ends

DATA32 segment
DATA32START label byte
DATA32 ends

BSS32 segment
BSS32START label byte
BSS32 ends

CONST32_RO segment
CONST32_ROSTART label byte
CONST32_RO ends
END

