; $Id: exceptutil.asm,v 1.18 2001-07-20 15:33:29 sandervl Exp $

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

DATA32	segment dword use32 public 'DATA'

CONST32_RO	segment
	align 04h
@CBE8	db "KERNEL32: Calling handle"
db "r at %p code=%lx flags=%"
db "lx",0ah,0h
@CBE9	db "KERNEL32: Handler return"
db "ed %lx",0ah,0h
CONST32_RO	ends
DATA32	ends

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

        push    dword ptr [ebp+12]
        mov     eax, dword ptr [ebp+8]
        call    eax

        mov     esp, ebp
        pop     ebp
        ret
_AsmCallThreadHandler endp

        PUBLIC _CallEntryPoint
_CallEntryPoint proc near
        push	ebp
        mov	ebp, esp

        mov     eax, esp
        sub     eax, 16
        and     eax, 0FFFFFFF0h
        add     eax, 4
        mov     esp, eax

        push    dword ptr [ebp+12]
        mov     eax, dword ptr [ebp+8]
        call    eax

        mov     esp, ebp
        pop     ebp
        ret
_CallEntryPoint endp


; 281 static DWORD EXC_CallHandler( WINEXCEPTION_RECORD *record, WINEXCEPTION_FRAME *frame,
        EXTRN WriteLog:PROC
        EXTRN _GetThreadTEB@0:PROC
IFDEF DEBUG
        EXTRN DbgEnabledKERNEL32:DWORD
ENDIF

EXC_push_frame__FP19_WINEXCEPTION_FRAME	proc
	push	ebp
	mov	ebp,esp
	sub	esp,04h
	mov	[ebp+08h],eax;	frame

; 132     TEB *teb = GetThreadTEB();
	call	_GetThreadTEB@0
	mov	[ebp-04h],eax;	teb

; 133     frame->Prev = (PWINEXCEPTION_FRAME)teb->except;
	mov	ecx,[ebp-04h];	teb
	mov	ecx,[ecx]
	mov	eax,[ebp+08h];	frame
	mov	[eax],ecx

; 134     teb->except = frame;
	mov	eax,[ebp-04h];	teb
	mov	ecx,[ebp+08h];	frame
	mov	[eax],ecx

; 135     return frame->Prev;
	mov	eax,[ebp+08h];	frame
	mov	eax,[eax]
	leave	
	ret	
EXC_push_frame__FP19_WINEXCEPTION_FRAME	endp

; 138 static inline WINEXCEPTION_FRAME * EXC_pop_frame( WINEXCEPTION_FRAME *frame )
	align 04h

EXC_pop_frame__FP19_WINEXCEPTION_FRAME	proc
	push	ebp
	mov	ebp,esp
	sub	esp,04h
	mov	[ebp+08h],eax;	frame

; 141     TEB *teb = GetThreadTEB();
	call	_GetThreadTEB@0
	mov	[ebp-04h],eax;	teb

; 142     teb->except = frame->Prev;
	mov	ecx,[ebp+08h];	frame
	mov	ecx,[ecx]
	mov	eax,[ebp-04h];	teb
	mov	[eax],ecx

; 143     return frame->Prev;
	mov	eax,[ebp+08h];	frame
	mov	eax,[eax]
	leave	
	ret	
EXC_pop_frame__FP19_WINEXCEPTION_FRAME	endp

	align 04h
        PUBLIC EXC_CallHandler__FP20_WINEXCEPTION_RECORDP19_WINEXCEPTION_FRAMEP10WINCONTEXTPP19_WINEXCEPTION_FRAMEPFP20_WINEXCEPTION_RECORDP19_WINEXCEPTION_FRAMEP10WINCONTEXTPv_UlT5

EXC_CallHandler__FP20_WINEXCEPTION_RECORDP19_WINEXCEPTION_FRAMEP10WINCONTEXTPP19_WINEXCEPTION_FRAMEPFP20_WINEXCEPTION_RECORDP19_WINEXCEPTION_FRAMEP10WINCONTEXTPv_UlT5	proc
	push	ebp
	mov	ebp,esp
	sub	esp,010h
	sub	esp,04h
	mov	[ebp+08h],eax;	record
	mov	[ebp+0ch],edx;	frame
	mov	[ebp+010h],ecx;	context

; 296     newframe.frame.Handler = nested_handler;
	mov	eax,[ebp+01ch];	nested_handler
	mov	[ebp-08h],eax;	newframe

; 297     newframe.prevFrame     = frame;
	mov	eax,[ebp+0ch];	frame
	mov	[ebp-04h],eax;	newframe

; 298     EXC_push_frame( &newframe.frame );
	lea	eax,[ebp-0ch];	newframe
	call	EXC_push_frame__FP19_WINEXCEPTION_FRAME

; 299     dprintf(("KERNEL32: Calling handler at %p code=%lx flags=%lx\n",
IFDEF DEBUG
	cmp	word ptr  DbgEnabledKERNEL32+020h,01h
	jne	@BLBL20
	mov	eax,[ebp+08h];	record
	push	dword ptr [eax+04h]
	mov	eax,[ebp+08h];	record
	push	dword ptr [eax]
	push	dword ptr [ebp+018h];	handler
	push	offset FLAT:@CBE8
	call	WriteLog
	add	esp,010h
ENDIF

; 300            handler, record->ExceptionCode, record->ExceptionFlags));
@BLBL20:

; 301     ret = handler( record, frame, context, dispatcher );
	push	dword ptr [ebp+014h];	dispatcher
	push	dword ptr [ebp+010h];	context
	push	dword ptr [ebp+0ch];	frame
	push	dword ptr [ebp+08h];	record
	call	dword ptr [ebp+018h];	handler
	mov	[ebp-010h],eax;	ret

IFDEF DEBUG
; 302     dprintf(("KERNEL32: Handler returned %lx\n", ret));
	cmp	word ptr  DbgEnabledKERNEL32+020h,01h
	jne	@BLBL21
	push	dword ptr [ebp-010h];	ret
	push	offset FLAT:@CBE9
	call	WriteLog
	add	esp,08h
@BLBL21:
ENDIF

; 303     EXC_pop_frame( &newframe.frame );
	lea	eax,[ebp-0ch];	newframe
	call	EXC_pop_frame__FP19_WINEXCEPTION_FRAME

; 304     return ret;
	mov	eax,[ebp-010h];	ret
	add	esp,04h
	leave	
	ret	
EXC_CallHandler__FP20_WINEXCEPTION_RECORDP19_WINEXCEPTION_FRAMEP10WINCONTEXTPP19_WINEXCEPTION_FRAMEPFP20_WINEXCEPTION_RECORDP19_WINEXCEPTION_FRAMEP10WINCONTEXTPv_UlT5	endp

CODE32          ENDS

                END
