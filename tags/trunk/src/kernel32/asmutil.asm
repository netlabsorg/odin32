; $Id: asmutil.asm,v 1.7 2005-02-08 20:48:46 sao2l02 Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 1998 Sander van Leeuwen
; *
; */
.386p
                NAME    except

DATA32	segment dword use32 public 'DATA'
DATA32	ends
CONST32_RO	segment dword use32 public 'CONST'
CONST32_RO	ends
BSS32	segment dword use32 public 'BSS'
BSS32	ends
DGROUP	group BSS32, DATA32
	assume	cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT

extrn   Dos32TIB:abs

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        PUBLIC getEAX
        PUBLIC getEBX
getEAX  proc near
        ret
getEAX  endp

public	getEDX
getEDX	proc	near
	mov	EAX, EDX
	ret
endp

getEBX  proc near
        mov  eax, ebx
        ret
getEBX  endp

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

        PUBLIC getCS
getCS   proc near
        mov     eax, cs
        ret
getCS   endp

        PUBLIC getDS
getDS   proc near
        mov     eax, ds
        ret
getDS   endp

        PUBLIC SetReturnFS
SetReturnFS proc near
        push    fs
        mov     eax, [esp+8]
        mov     fs, eax
        pop     eax
        ret
SetReturnFS endp

        PUBLIC getSS
getSS   proc near
        mov     eax, ss
        ret
getSS   endp

        PUBLIC getES
getES   proc near
        mov     eax, es
        ret
getES   endp

        PUBLIC getGS
getGS   proc near
        mov     eax, gs
        ret
getGS   endp

        PUBLIC getESP
getESP  proc near
        mov     eax, esp
        ret
getESP  endp

        PUBLIC RestoreOS2FS
RestoreOS2FS proc near
        push    Dos32TIB
        mov     eax, fs
        pop     fs
        ret
RestoreOS2FS endp

        PUBLIC _Mul32x32to64
_Mul32x32to64 proc near
        push    ebp
        mov     ebp, esp
        push    eax
        push    edx
        push    edi

        mov     edi, [ebp+8]    ;64 bits result
        mov     eax, [ebp+12]   ;op1
        mov     edx, [ebp+16]   ;op2
        mul     edx
        mov     [edi], eax
        mov     [edi+4], edx

        pop     edi
        pop     edx
        pop     eax
        pop     ebp
        ret
_Mul32x32to64 endp

	align 4h

	public _Sub64
_Sub64	proc

; 34    c = a - b;
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        push    edx

        mov     edi, [ebp+10h]  ;&c
        mov     esi, [ebp+08h]  ;&a
	mov	eax,[esi]       ;a.low
	mov	edx,[esi+04h]   ;a.high
        mov     esi, [ebp+0ch]  ;&b
	sub	eax,[esi]       ;b.low
	sbb	edx,[esi+04h]   ;b.high

        mov     [edi], eax      ;c.low
        mov     [edi+4], edx    ;c.high

        pop     edx
        pop     edi
        pop     esi
        leave
	ret	
_Sub64	endp

	align 4h

	public _Add64
_Add64	proc

; 25     c = a + b;
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        push    edx

        mov     edi, [ebp+10h]  ;&c
        mov     esi, [ebp+08h]  ;&a
	mov	eax,[esi]       ;a.low
	mov	edx,[esi+04h]   ;a.high
        mov     esi, [ebp+0ch]  ;&b
	add	eax,[esi]       ;b.low
	adc	edx,[esi+04h]   ;b.high

        mov     [edi], eax      ;c.low
        mov     [edi+4], edx    ;c.high

        pop     edx
        pop     edi
        pop     esi
        leave
	ret	

_Add64	endp


	align 4h

	public _set_bit
;int CDECL set_bit(int bitnr, void *addr);
_set_bit proc near
    push esi

    mov  esi, [esp+12]
    mov  eax, [esp+8]

    bts  dword ptr [esi], eax
    setc al
    movzx eax, al

    pop  esi
    ret
_set_bit endp

	align 4h

	public _test_bit
;int CDECL test_bit(int bitnr, void *addr);
_test_bit proc near
    push esi

    mov  esi, [esp+12]
    mov  eax, [esp+8]

    bt   dword ptr [esi], eax
    setc al
    movzx eax, al

    pop  esi
    ret
_test_bit endp

	public _clear_bit
;int CDECL clear_bit(int bitnr, void *addr);
_clear_bit proc near
    push esi

    mov  esi, [esp+12]
    mov  eax, [esp+8]

    btr  dword ptr [esi], eax
    setc al
    movzx eax, al

    pop  esi
    ret
_clear_bit endp

	public _search_zero_bit
;int CDECL search_zero_bit(int bitcount, void *addr);
_search_zero_bit proc near
    push esi
    push edx

    mov  esi, [esp+16]
    mov  edx, [esp+12]
    inc  edx
loop:
    add  esi, 4
    mov  eax, -1
    dec  edx
    jz   short none
    xor  eax, dword ptr [esi - 4]
    jz   loop
    bsf  eax, eax
    jz   short loop
    sub  esi, 4 
    sub  esi, [esp+16]
    shl  esi, 5-2
    add  eax, esi
    jmp  short found    
none:
    setnz al
    movsx eax, al
found:

    pop  edx
    pop  esi
    ret
_search_zero_bit endp

CODE32          ENDS

                END
