; $Id: asmutil.asm,v 1.3 2003-03-27 14:13:10 sandervl Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 1998 Sander van Leeuwen
; *
; */
.386p
                NAME    except

DATA32  segment dword use32 public 'DATA'
DATA32  ends
CONST32_RO      segment dword use32 public 'CONST'
CONST32_RO      ends
BSS32   segment dword use32 public 'BSS'
BSS32   ends
DGROUP  group BSS32, DATA32
        assume  cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT

extrn   Dos32TIB:abs

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        PUBLIC _getEAX
_getEAX proc near
        ret
_getEAX endp

        PUBLIC  _getEDX
_getEDX proc    near
        mov     EAX, EDX
        ret
_getEDX endp

        PUBLIC _getEBX
_getEBX proc near
        mov  eax, ebx
        ret
_getEBX endp

IFNDEF __GNUC__

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
SetFS  endp

ENDIF ; __GNUC__

        PUBLIC _getCS
_getCS  proc near
        mov     eax, cs
        ret
_getCS  endp

        PUBLIC _getDS
_getDS  proc near
        mov     eax, ds
        ret
_getDS  endp

IFNDEF __GNUC__

        PUBLIC SetReturnFS
SetReturnFS proc near
        push    fs
        mov     eax, [esp+8]
        mov     fs, eax
        pop     eax
        ret
SetReturnFS endp

ENDIF ; __GNUC__

        PUBLIC _getSS
_getSS  proc near
        mov     ax, ss
        ret
_getSS  endp

        PUBLIC _getES
_getES  proc near
        mov     eax, es
        ret
_getES  endp

        PUBLIC _getGS
_getGS  proc near
        mov     eax, gs
        ret
_getGS  endp

        PUBLIC _getESP
_getESP proc near
        mov     eax, esp
        ret
_getESP endp

IFNDEF __GNUC__

        PUBLIC RestoreOS2FS
RestoreOS2FS proc near
        push    Dos32TIB
        mov     ax, fs
        pop     fs
        ret
RestoreOS2FS endp

ENDIF ; __GNUC__

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
_Sub64  proc

; 34    c = a - b;
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        push    edx

        mov     edi, [ebp+10h]  ;&c
        mov     esi, [ebp+08h]  ;&a
        mov     eax,[esi]       ;a.low
        mov     edx,[esi+04h]   ;a.high
        mov     esi, [ebp+0ch]  ;&b
        sub     eax,[esi]       ;b.low
        sbb     edx,[esi+04h]   ;b.high

        mov     [edi], eax      ;c.low
        mov     [edi+4], edx    ;c.high

        pop     edx
        pop     edi
        pop     esi
        leave
        ret
_Sub64  endp

        align 4h

        public _Add64
_Add64  proc

; 25     c = a + b;
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        push    edx

        mov     edi, [ebp+10h]  ;&c
        mov     esi, [ebp+08h]  ;&a
        mov     eax,[esi]       ;a.low
        mov     edx,[esi+04h]   ;a.high
        mov     esi, [ebp+0ch]  ;&b
        add     eax,[esi]       ;b.low
        adc     edx,[esi+04h]   ;b.high

        mov     [edi], eax      ;c.low
        mov     [edi+4], edx    ;c.high

        pop     edx
        pop     edi
        pop     esi
        leave
        ret

_Add64  endp


        align 4h

        public _set_bit
;void CDECL set_bit(int bitnr, void *addr);
_set_bit proc near
    push esi

    mov  esi, [esp+12]
    mov  eax, [esp+8]

    bts  dword ptr [esi], eax

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
;void CDECL clear_bit(int bitnr, void *addr);
_clear_bit proc near
    push esi

    mov  esi, [esp+12]
    mov  eax, [esp+8]

    btr  dword ptr [esi], eax

    pop  esi
    ret
_clear_bit endp

rdtsc           macro
                db      0Fh, 31h
endm

                public  GetPentiumTSC
GetPentiumTSC   proc    near
                mov     ecx , [esp + 4]
                rdtsc
                mov     [ecx] , eax
                mov     [ecx + 4] , edx
                xor     eax , eax
                ret
GetPentiumTSC   endp

CODE32          ENDS

                END
