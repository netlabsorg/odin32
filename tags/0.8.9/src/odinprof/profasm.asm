; $Id: profasm.asm,v 1.1 2001-11-22 10:43:59 phaller Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Execution Trace Profiler
; *
; * Copyright 2001 Patrick Haller <patrick.haller@innotek.de>
; *
; */

.586p
                NAME    profasm

extrn           C_ProfileHook32Enter: proc
extrn           C_ProfileHook32Exit: proc

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  DS:FLAT, SS:FLAT

		public odin_ProfileHook32
odin_ProfileHook32	proc near

                ; --------
                ; top half
                ; --------

                pushfd
                push eax  ; save altered registers
                push edx
                push ecx

                rdtsc     ; push the machine timestamp
                          ; (assuming at least Pentium CPU is available)
                push eax
                push edx
                mov  edx, dword ptr [esp+1Ch]
                push edx     ; push return address for bottom half
                mov  edx, dword ptr [esp+1Ch]
                push edx     ; push entry address of caller function

                ; ret_addr _System C_ProfilerHook32Enter(ULONG eip_function, ULONG ret, ULONGLONG tsc)
                call C_ProfileHook32Enter
                add esp,  10h

                ; restore the stack as it was before
                ; and replace the (saved) caller's address by the
                ; hook's bottom half address
                mov dword ptr [esp + 14h], eax

                pop ecx
                pop edx
                pop eax
                popfd

                ret
odin_ProfileHook32 endp


		public odin_ProfileHook32Bottom
odin_ProfileHook32Bottom proc near

                ; -----------
                ; bottom half
                ; -----------

                push 0    ; make room for virtual jump

                pushfd
                push eax  ; save altered registers
                push edx
                push ecx

                rdtsc     ; push the machine timestamp
                          ; (assuming at least Pentium CPU is available)
                push eax
                push edx

                ; ret_addr _System C_ProfilerHook32Exit(ULONGLONG tsc)
                call C_ProfileHook32Exit
                add esp,  08h

                ; restore the stack as it was before
                ; and virtually JUMP BACK to the caller's caller
                ; eax now contains the return address
                mov dword ptr [esp + 10h], eax

                pop  ecx
                pop  edx   ; restore altered registers
                pop  eax
                popfd

                ; this is a virtual jump!
                ret
odin_ProfileHook32Bottom	endp

		public _ProfileGetTimestamp
_ProfileGetTimestamp proc near

                push eax  ; save altered registers
                push edx
                push edi

                rdtsc     ; push the machine timestamp
                          ; (assuming at least Pentium CPU is available)
                mov edi, dword ptr [esp+14h]
                mov dword ptr [edi], eax
                mov edi, dword ptr [esp+10h]
                mov dword ptr [edi], edx

                pop edi
                pop edx
                pop eax

                ret
_ProfileGetTimestamp	endp


CODE32          ENDS
                END
