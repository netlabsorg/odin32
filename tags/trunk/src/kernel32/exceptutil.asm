; $Id: exceptutil.asm,v 1.13 2001-03-22 18:16:40 sandervl Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 1998 Sander van Leeuwen
; *
; */
.386p
                NAME    except

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'
        public  _RaiseException@16
        extrn   OS2RAISEEXCEPTION : near

_RaiseException@16 proc near
        push dword ptr [esp+4]  ;DWORD dwExceptionCode
        push dword ptr [esp+12] ;DWORD dwExceptionFlags
        push dword ptr [esp+20] ;DWORD cArguments
        push dword ptr [esp+28] ;DWORD *lpArguments
        push dword ptr [esp+16] ;return address
        push esp
        add  dword ptr [esp], 20
        push ebp
        pushfd
        push eax
        push ebx
        push ecx
        push edx
        push edi
        push esi
        xor  eax, eax
        mov  eax, cs
        push eax
        mov  eax, ds
        push eax
        mov  eax, es
        push eax
        mov  eax, fs
        push eax
        mov  eax, gs
        push eax
        mov  eax, ss
        push eax
        call OS2RAISEEXCEPTION

        ret 16      ;__stdcall
_RaiseException@16 endp

        public  _RtlUnwind@16
        extrn   OS2RTLUNWIND : near

_RtlUnwind@16 proc near
        push dword ptr [esp+4]  ;PWINEXCEPTION_FRAME  pEndFrame
        push dword ptr [esp+12] ;LPVOID unusedEip
        push dword ptr [esp+20] ;PWINEXCEPTION_RECORD pRecord
        push dword ptr [esp+28] ;DWORD  returnEax
        push dword ptr [esp+16] ;return address
        push esp
        add  dword ptr [esp], 20
        push ebp
        pushfd
        push eax
        push ebx
        push ecx
        push edx
        push edi
        push esi
        xor  eax, eax
        mov  eax, cs
        push eax
        mov  eax, ds
        push eax
        mov  eax, es
        push eax
        mov  eax, fs
        push eax
        mov  eax, gs
        push eax
        mov  eax, ss
        push eax
        call OS2RTLUNWIND

        ret 16      ;__stdcall
_RtlUnwind@16 endp


        PUBLIC QueryExceptionChain

QueryExceptionChain proc near
        mov  eax, fs:[0]
        ret
QueryExceptionChain endp

        PUBLIC GetExceptionRecord
GetExceptionRecord proc near
        push ebp
        mov  ebp, esp
        push fs
        push ebx

        mov  eax, [ebp+8]
        mov  fs, eax
        mov  ebx, [ebp+12]
        mov  eax, fs:[ebx]

        pop  ebx
        pop  fs
        pop  ebp
        ret
GetExceptionRecord endp

                PUBLIC ChangeTIBStack
ChangeTIBStack proc near
;       xor  eax, eax
        push ebx
        mov  eax, fs:[4]
        mov  ebx, fs:[8]
        add  ebx, 8
        mov  fs:[4], ebx
        mov  fs:[8], eax
        pop  ebx
        ret
ChangeTIBStack endp

        PUBLIC _SetExceptionChain

_SetExceptionChain proc near
        mov eax, dword ptr [esp+4]
        mov fs:[0], eax
        ret
_SetExceptionChain endp

        PUBLIC getEAX
        PUBLIC getEBX
getEAX  proc near
        ret
getEAX  endp

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
        mov     ax, ss
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
        push    150bh
        mov     ax, fs
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

        PUBLIC  _AsmCallThreadHandler
_AsmCallThreadHandler proc near
        push	ebp
        mov	ebp, esp

        push    [ebp+12]
        mov     eax, dword ptr [ebp+8]
        call    eax

        mov     esp, ebp
        pop     ebp
        ret
_AsmCallThreadHandler endp

CODE32          ENDS

                END
