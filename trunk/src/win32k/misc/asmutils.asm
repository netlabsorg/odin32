; $Id: asmutils.asm,v 1.4 2000-02-19 08:40:31 bird Exp $
;
; asmutils - assembly utility functions
;
; Copyright (c) 1998-1999 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Include files
;
    include devsegdf.inc

;
; Exported symbols
;
    public GetCS
    public GetDS
    public GetES
    public GetFS
    public GetGS
    public GetSS
    public Int3
;    public _memrevmov@12
;    public _memmov@12
;    public DisableInterrupts
;    public EnableInterrupts


CODE32 segment
    assume CS:CODE32, DS:FLAT, SS:NOTHING

GetCS proc near
    xor eax,eax
    mov ax,cs
    ret
GetCS endp


GetDS proc near
    xor eax,eax
    mov ax,ds
    ret
GetDS endp


GetES proc near
    xor eax,eax
    mov ax,es
    ret
GetES endp


GetFS proc near
    xor eax,eax
    mov ax,fs
    ret
GetFS endp


GetGS PROC NEAR
    xor eax,eax
    mov ax,gs
    ret
GetGS ENDP


GetSS PROC NEAR
    xor eax,eax
    mov ax,ss
    ret
GetSS ENDP


Int3 PROC NEAR
    push ebp
    mov ebp,esp
    int 3
    pop ebp
    ret
Int3 ENDP


;DisableInterrupts PROC NEAR
;        cli
;        ret
;DisableInterrupts ENDP

;EnableInterrupts PROC NEAR
;        sti
;        ret
;EnableInterrupts ENDP

if 0
;void __stdcall memrevmov(void *pTo, void* pFrom, unsigned int Len);
pTo     EQU dword ptr [ebp+08]
pFrom   EQU dword ptr [ebp+12]
Len     EQU dword ptr [ebp+16]
_memrevmov@12 PROC NEAR
    push ebp
    mov ebp,esp
    push esi
    push edi


    mov edi,pTo
    mov esi,pFrom
    mov ecx,Len

    ;test if (pTo+4 <= pFrom) then use dword_move else byte_move
    dec edi
    dec esi
    mov eax,edi
    sub eax,esi
    cmp eax,4
    jg memrevmov_loopbyte
 memrevmov_loopdword::
    mov eax,[esi+ecx]
    mov [edi+ecx],eax
    sub ecx,4
    cmp ecx,4
     jg memrevmov_loopdword

 memrevmov_loopbyte::
    mov al,[esi+ecx]
    mov [edi+ecx],al
   loop memrevmov_loopbyte

    pop edi
    pop esi
    pop ebp
    ret 12
_memrevmov@12 ENDP

;void __stdcall memmov(void *p, signed int off, unsigned int len);
p       EQU dword ptr [ebp+08]
off     EQU dword ptr [ebp+12]
len     EQU dword ptr [ebp+16]
_memmov@12 PROC NEAR
    push ebp
    mov ebp,esp
    push esi
    push edi

    mov ecx,len
    mov esi,p
    mov edi,esi
    mov eax,off
    test eax, 80000000h
     ja memmov_neg

    add edi,eax
    dec edi
    dec esi

    cmp eax,4
    jg memrevmov_loopbyte
 memmov_loopdword:
    mov eax,[esi+ecx]
    mov [edi+ecx],eax
    sub ecx,4
    cmp ecx,4
     jl memrevmov_loopdword

 memmov_loopbyte:
    mov al,[esi+ecx]
    mov [edi+ecx],al
   loop memrevmov_loopbyte

 memmov_end:
    pop edi
    pop esi
    pop ebp
    ret 12

 memmov_neg:
    ;this don't work!!!
    neg eax
    sub edi,eax

    cmp eax,4
     jg memmov_neg_byte
    and ecx,NOT 03h
    rep movsd

    mov ecx,len
    and ecx,03h
  memmov_neg_byte:
    rep movsb

    jmp memmov_end
_memmov@12 ENDP
endif

CODE32 ends
end
