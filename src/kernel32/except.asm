; $Id: except.asm,v 1.4 1999-06-19 13:57:51 sandervl Exp $

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
                ASSUME  CS:FLAT ,DS:FLAT,SS:FLAT
        public  RaiseExceptionAsm
        extrn   OS2RAISEEXCEPTION : near

RaiseExceptionAsm proc near
        push dword ptr [esp+4]  ;DWORD dwExceptionCode
        push dword ptr [esp+12] ;DWORD dwExceptionFlags
        push dword ptr [esp+20] ;DWORD cArguments
        push dword ptr [esp+28] ;DWORD *lpArguments
        push dword ptr [esp+16] ;return address
        push esp
        sub  dword ptr [esp-4], 20
        push ebp
        pushfd
        push eax
        push ebx
        push ecx
        push edx
        push edi
        push esi
        xor  eax, eax
        mov  eax, cs    ;does 'push cs' push a dword??
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

        ret 20      ;__stdcall
RaiseExceptionAsm endp

        public  RtlUnwindAsm
        extrn   OS2RTLUNWIND : near

RtlUnwindAsm proc near
        push dword ptr [esp+4]  ;PWINEXCEPTION_FRAME  pEndFrame
        push dword ptr [esp+12] ;LPVOID unusedEip
        push dword ptr [esp+20] ;PWINEXCEPTION_RECORD pRecord
        push dword ptr [esp+28] ;DWORD  returnEax
        push dword ptr [esp+16] ;return address
        push esp
        sub  dword ptr [esp-4], 20
        push ebp
        pushfd
        push eax
        push ebx
        push ecx
        push edx
        push edi
        push esi
        xor  eax, eax
        mov  eax, cs    ;does 'push cs' push a dword??
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

        ret 20      ;__stdcall
RtlUnwindAsm endp


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

                PUBLIC DisableFPUExceptions
DisableFPUExceptions proc near
        push  eax
        push  67Fh
        fldcw word ptr [esp]
        pop   eax
        pop   eax
        ret
DisableFPUExceptions endp

        PUBLIC GetDllEntryPoint

GetDllEntryPoint proc near
        mov   eax, [ebp + 4]    ; return address in win32 dll
        ret
GetDllEntryPoint endp

        PUBLIC getEAX
        PUBLIC getEBX
getEAX 	proc near
        ret
getEAX 	endp

getEBX 	proc near
        mov  eax, ebx
        ret
getEBX 	endp

        PUBLIC GetFS
GetFS  	proc near
	mov	eax, fs
	ret
GetFS  	endp

        PUBLIC SetFS
SetFS	proc near
	mov  	eax, [esp+4]
	mov	fs, eax
	ret
SetFS	endp

        PUBLIC getSS
getSS	proc near
	mov	ax, ss
	ret
getSS	endp

CODE32          ENDS

                END
