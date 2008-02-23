; $Id: d32hlp.asm,v 1.6 2001-07-08 02:53:52 bird Exp $
;
; d32hlp - 32-bit Device Driver Helper Function.
;
; Copyright (c) 1999 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p


;
; Include files
;
    include devsegdf.inc
    include devhlp.inc


;
; Exported symbols
;
    public D32Hlp_VirtToLin
    public D32Hlp_VirtToLin2
    public D32Hlp_VirtToLin3
    public D32Hlp_GetDOSVar
    public D32Hlp_Yield
    public D32Hlp_VMAlloc
    public D32Hlp_VMFree
    public D32Hlp_VMSetMem
    public D32Hlp_VMLock
    public D32Hlp_VMLock2
    public D32Hlp_VMUnLock


;
; extrns
;
    extrn _Device_Help:dword


CODE32 segment
    assume cs:CODE32, ds:flat, ss:nothing, es:nothing
    .386p

;PVOID  D32HLPCALL D32Hlp_VirtToLin(ULONG  ulPtr16); /* eax */
; Sideeffect: edx holds the error code on error.
D32Hlp_VirtToLin proc near
    push    esi

    movzx   esi, ax
    shr     eax, 16
    mov     dl, DevHlp_VirtToLin
    jmp     far ptr CODE16:Thunk16_VirtToLin
Thunk32_VirtToLin::
    jnc     Finished
    mov     edx, eax
    xor     eax, eax

Finished:
    pop     esi
    ret
D32Hlp_VirtToLin endp


;PVOID  D32HLPCALL D32Hlp_VirtToLin2(USHORT usSelector, /*  ax */
;                                    ULONG  ulOffset);  /* edx */
; Sideeffect: edx holds the error code on error.
D32Hlp_VirtToLin2 proc near
    push    esi

    mov     esi, edx
    mov     dl, DevHlp_VirtToLin
    jmp     far ptr CODE16:Thunk16_VirtToLin2
Thunk32_VirtToLin2::
    jnc     Finished
    mov     edx, eax
    xor     eax, eax

Finished:
    pop     esi
    ret
D32Hlp_VirtToLin2 endp


;D32RET D32HLPCALL D32Hlp_VirtToLin3(USHORT usSelector,     /*  ax */
;                                    ULONG  ulOffset,       /* edx */
;                                    PPVOID ppvLinAddress); /* ecx */
D32Hlp_VirtToLin3 proc near
    push    esi

    mov     esi, edx
    mov     dl, DevHlp_VirtToLin
    jmp     far ptr CODE16:Thunk16_VirtToLin3
Thunk32_VirtToLin3::
    jc Finished
    mov     [ecx], eax
    xor     eax, eax

Finished:
    pop     esi
    ret
D32Hlp_VirtToLin3 endp


;
; Jump table used by D32Hlp_GetDOSVar
;
DosVarThunkTab  LABEL DWORD
    dd  0                               ; 0 - Reserved
    dd  FLAT:Load1600Thunk              ; 1 - GIS
    dd  FLAT:Load1616Thunk              ; 2 - LIS
    dd  0                               ; 3 - Reserved
    dd  FLAT:Load1616                   ; 4 - VectorSDF
    dd  FLAT:Load1616                   ; 5 - VectorReboot
    dd  FLAT:Load1616                   ; 6 - VectorMSATS
    dd  FLAT:ThunkPtr                   ; 7 - YieldFlag (Resched)
    dd  FLAT:ThunkPtr                   ; 8 - TCYieldFlag (TCResched)
    dd  FLAT:ThunkPtr                   ; 9 - DOSTable (yeah, reserved)
    dd  FLAT:Load1616                   ; a - VectorDEKKO (yeah, reserved)
    dd  FLAT:ThunkPtr                   ; b - CodePgBuff
    dd  FLAT:Load1616                   ; c - VectorRIPL
    dd  FLAT:ThunkPtr                   ; d - InterruptLevel
    dd  FLAT:ThunkPtr                   ; e - DevClassTables
    dd  FLAT:ThunkPtr                   ; f - DMQS_Sel
    dd  FLAT:ThunkPtr                   ;10 - APMInfo
    dd  FLAT:LoadWord                   ;11 - APM_Length (length of above structure)
DosVarThunkTabSize equ ($ - DosVarThunkTab) / 4


;PVOID  D32HLPCALL D32Hlp_GetDOSVar(ULONG ulVarNumber,  /* eax */
;                                   ULONG ulVarMember); /* edx */
;
; Note that we do post processing of the value....!
;
D32Hlp_GetDOSVar proc near
    ASSUME DS:FLAT, ES:NOTHING
    push    ebx
    push    es
    push    ds
    push    ds
    pop     es                          ; paranoia
    ASSUME  es:FLAT
    push    eax
    mov     ecx, edx
    mov     dl, DevHlp_GetDOSVar
    jmp     far ptr CODE16:Thunk16_GetDOSVar
Thunk32_GetDOSVar::
    jc  Error
    mov     ds, ax
    pop     eax                         ; Index...
    cmp     eax, DosVarThunkTabSize
    jg      Error
    and     ebx, 0000ffffh              ; (ds:ebx is ptr to be processed)
    jmp     es:DosVarThunkTab[eax*4]

; Load Word at ds:bx
LoadWord::
    xor     eax, eax
    mov     ax, word ptr [bx]
    jmp Finished

; Load selector at ds:bx and thunk it.
Load1600Thunk::
    xor     eax, eax
    mov     ax, word ptr [bx]
    rol     ax, 16
    jmp ThunkIt

; Load 16:16 ptr at ds:bx and thunk it.
Load1616Thunk::
    mov     eax, dword ptr [ebx]
    jmp ThunkIt

; Load 16:16 ptr at ds:bx and return it.
Load1616::
    mov     eax, dword ptr [ebx]
    jmp Finished

; Thunk the ds:bx ptr.
ThunkPtr::
    mov     ax, ds
    shl     eax, 16
    mov     ax, bx

; Thunks 16:16 ptr in eax
ThunkIt::
    pop     ds
    call    D32Hlp_VirtToLin
    jmp Finished2

Error:
    add     esp, 4
    xor     eax, eax

Finished:
    pop     ds
Finished2:
    pop     es
    pop     ebx
    ret
D32Hlp_GetDOSVar endp



;VOID   D32HLPCALL D32Hlp_Yield(VOID);
D32Hlp_Yield proc near
    mov     dl, DevHlp_Yield
    jmp     far ptr CODE16:Thunk16_Yield
Thunk32_Yield::
    ret
D32Hlp_Yield endp



;PVOID  D32HLPCALL D32Hlp_VMAlloc(ULONG  flFlags,    /* eax */
;                                 ULONG  cbSize,     /* edx */
;                                 ULONG  ulPhysAddr);/* ecx */
D32Hlp_VMAlloc proc near
    push    edi

    mov     edi, ecx
    mov     ecx, edx

    mov     dl, DevHlp_VMAlloc
    jmp     far ptr CODE16:Thunk16_VMAlloc
Thunk32_VMAlloc::
    jnc Finished
    xor     eax, eax

Finished:
    pop     edi
    ret
D32Hlp_VMAlloc endp


;D32RET D32HLPCALL D32Hlp_VMFree(PVOID pvAddress); /* eax */
D32Hlp_VMFree proc near
    mov     dl, DevHlp_VMFree
    jmp     far ptr CODE16:Thunk16_VMFree
Thunk32_VMFree::
    jc      Finished
    xor     eax, eax
Finished:
    ret
D32Hlp_VMFree endp


;D32RET D32HLPCALL D32Hlp_VMSetMem(PVOID pvAddress, /* eax */
;                                  ULONG cbSize,    /* edx */
;                                  ULONG flFlags);  /* ecx */
D32Hlp_VMSetMem proc near
    push    ebx

    mov     ebx, eax
    mov     eax, ecx
    mov     ecx, edx

    mov     dl, DevHlp_VMSetMem
    jmp     far ptr CODE16:Thunk16_VMSetMem
Thunk32_VMSetMem::

    pop     ebx
    ret
D32Hlp_VMSetMem endp


;D32RET D32HLPCALL D32Hlp_VMLock(PVOID   pvAddress,         /* eax (ebp + 08h) */
;                                ULONG   ulLength,          /* edx (ebp + 0Ch)*/
;                                ULONG   flFlags,           /* ecx (ebp + 10h)*/
;                                PVOID   pvPagelist,        /* ebp + 14h */
;                                PLOCKHANDLE  pLockHandle,  /* ebp + 18h */
;                                PULONG  pulPageListCount); /* ebp + 1Ch */
D32Hlp_VMLock proc near
    push    ebp
    mov     ebp, esp
    push    esi
    push    edi
    push    ebx

    mov     edi, [ebp + 14h]
    mov     esi, [ebp + 18h]
    mov     ebx, eax
    mov     eax, ecx
    mov     ecx, edx

    mov     dl, DevHlp_VMLock
    jmp     far ptr CODE16:Thunk16_VMLock
Thunk32_VMLock::
    jc Finished
    mov     ebx, [ebp + 1Ch]
    or      ebx, ebx
    jz Success
    mov     [ebx], eax

Success:
    xor     eax, eax

Finished:
    pop     ebx
    pop     edi
    pop     esi
    pop     ebp
    ret
D32Hlp_VMLock endp


;D32RET D32HLPCALL D32Hlp_VMLock2(PVOID  pvAddress,         /* eax (ebp + 08h) */
;                                ULONG   ulLength,          /* edx (ebp + 0Ch)*/
;                                ULONG   flFlags,           /* ecx (ebp + 10h)*/
;                                PLOCKHANDLE  pLockHandle); /* ebp + 14h */
D32Hlp_VMLock2 proc near
    push    ebp
    mov     ebp, esp
    push    esi
    push    edi
    push    ebx

    mov     edi, 0ffffffffh
    mov     esi, [ebp + 14h]
    mov     ebx, eax
    mov     eax, ecx
    mov     ecx, edx

    mov     dl, DevHlp_VMLock
    jmp     far ptr CODE16:Thunk16_VMLock2
Thunk32_VMLock2::
    jc Finished
    xor     eax, eax

Finished:
    pop     ebx
    pop     edi
    pop     esi
    pop     ebp
    ret
D32Hlp_VMLock2 endp


;D32RET D32HLPCALL D32Hlp_VMUnLock(PLOCKHANDLE pLockHandle);    /* eax */
D32Hlp_VMUnLock proc near
    push    esi

    mov     esi, eax
    mov     dl, DevHlp_VMUnlock
    jmp     far ptr CODE16:Thunk16_VMUnLock
Thunk32_VMUnLock::
    jc      Finished
    xor     eax, eax

Finished:
    pop     esi
    ret
D32Hlp_VMUnLock endp

CODE32 ends





;CODE16 segment
CODE16 segment
    assume cs:CODE16, ds:FLAT

Thunk16_VirtToLin:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VirtToLin

Thunk16_VirtToLin2:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VirtToLin2

Thunk16_VirtToLin3:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VirtToLin3

Thunk16_GetDOSVar:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_GetDOSVar

Thunk16_Yield:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_Yield

Thunk16_VMAlloc:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VMAlloc

Thunk16_VMFree:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VMFree

Thunk16_VMLock:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VMLock

Thunk16_VMLock2:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VMLock2

Thunk16_VMUnLock:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VMUnLock

Thunk16_VMSetMem:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_VMSetMem

CODE16 ends

end
