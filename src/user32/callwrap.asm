; $Id: callwrap.asm,v 1.1 2004-01-11 12:03:12 sandervl Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 2003 Innotek Systemberatung GmbH (sandervl@innotek.de)
; *
; */
.386p
                NAME    helper

DATA32	segment dword use32 public 'DATA'
DATA32	ends
CONST32_RO	segment dword use32 public 'CONST'
CONST32_RO	ends
BSS32	segment dword use32 public 'BSS'
BSS32	ends
DGROUP	group BSS32, DATA32
	assume	cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT

DATA32	segment dword use32 public 'DATA'

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        PUBLIC _WrapCallback2
; WrapCallback(PFN lpfnFunction, DWORD param1, DWORD param2)
_WrapCallback2 proc near
        push  ebp
        push  ebx
        push  ecx
        push  edx
        push  edi
        push  esi
        mov   ebp, esp

        push  dword ptr [ebp+36]
        push  dword ptr [ebp+32]
        call  dword ptr [ebp+28]

        mov   esp, ebp

        pop   esi
        pop   edi
        pop   edx
        pop   ecx
        pop   ebx

        pop   ebp     
        ret
_WrapCallback2 endp


CODE32          ENDS

                END
