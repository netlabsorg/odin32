; $Id: devfirst.asm,v 1.4 2000-02-21 04:45:46 bird Exp $
;
; DevFirst - entrypoint and segment definitions
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
    include sas.inc

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
    public _CallGetOTEs32
    public _CallElfIOCtl
    public _CallWin32kIOCtl
    public _SSToDS_16a
    public GetOS2KrnlMTE


;
; Externs
;
    extrn _TKSSBase16:dword
    extrn GETOTES32:FAR
    extrn ELFIOCTL:FAR
    extrn WIN32KIOCTL:FAR
    .286p
    extrn _strategy:near


CODE16 segment
    ASSUME CS:CODE16, DS:DATA16, ES:NOTHING, SS:NOTHING

CODE16START label byte

    .286p
;$win32ki entry point
_strategyAsm0:
    push    0
    jmp     _strategyAsm

;$win32k entry point
_strategyAsm1:
    push    1
    jmp     _strategyAsm

;;
; Stub which pushes parameters onto the stack and call the 16-bit C strategy routine.
; @returns   returns the return value of strategy(...)
; @author    knut st. osmundsen
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
    retf
_strategyAsm endp


    .386p
;;
; Thunk procedure for R0Init32.
; @cproto    USHORT NEAR CallGetOTEs32(ULONG addressOTEBuf);
; @returns   Same as GetOTEs32.
; @param     address of OTEBuf  32-bit pointer to request data.
; @status    completely implemented.
; @author    knut st. osmundsen
_CallGetOTEs32 PROC NEAR
    ASSUME CS:CODE16
    push    ds
    push    word ptr [esp+6]            ; push high word.
    push    word ptr [esp+6]            ; push low word.
    call    far ptr FLAT:GETOTES32
    pop     ds
    retn
_CallGetOTEs32 ENDP




;;
; Thunk procedure for .
; @cproto    USHORT NEAR CallElfIOCtl(void);
; @returns   Same as ElfIOCtl
; @param     address of IOCtl request packet (32-bit pointer).
; @status    completely implemented.
; @author    knut st. osmundsen
_CallElfIOCtl PROC NEAR
    ASSUME CS:CODE16
    push    ds
    push    word ptr [esp+6]            ; push high word.
    push    word ptr [esp+6]            ; push low word.
    call    far ptr FLAT:ELFIOCTL
    pop     ds
    retn
_CallElfIOCtl ENDP

;;
; Thunk procedure for .
; @cproto    USHORT NEAR CallWin32kIOCtl(void);
; @returns   Same as Win32kIOCtl
; @param     address of IOCtl request packet (32-bit pointer).
; @status    completely implemented.
; @author    knut st. osmundsen
_CallWin32kIOCtl PROC NEAR
    ASSUME CS:CODE16
    push    ds
    push    word ptr [esp+6]            ; push high word.
    push    word ptr [esp+6]            ; push low word.
    call    far ptr FLAT:WIN32KIOCTL
    pop     ds
    retn
_CallWin32kIOCtl ENDP



;;
; SSToDS - stack pointer to Flat pointer.
; @cproto    extern LIN   SSToDS_16a(void NEAR *pStackVar);
; @returns   ax:dx  makes up a 32-bit flat pointer to stack.
; @param     pStackVar  Stack pointer which is to be made a flat pointer.
; @equiv     SSToDS in 32-bit code.
; @sketch    Get Flat CS
;            Get TKSSBase address. (FLAT)
;            return *TKSSBase + pStackVar.
; @status    completely  implemented.
; @author    knut st. osmundsen
; @remark    es is cs, not ds!
_SSToDS_16a proc near
    assume CS:CODE16, DS:DATA16, ES:NOTHING
    mov     edx, ds:_TKSSBase16         ; get pointer held by _TKSSBase16 (pointer to stack base)
    call    far ptr FLAT:far_getCS      ; get flat selector.
    push    es
    mov     es,  ax
    assume  es:FLAT
    mov     eax, es:[edx]               ; get pointer to stack base
    pop     es
    movzx   edx, word ptr ss:[esp + 2]  ; 16-bit stack pointer (parameter)
    add     eax, edx                    ; 32-bit stack pointer in eax
    mov     edx, eax
    shr     edx, 16                     ; dx high 16-bit of 32-bit stack pointer.
    ret
_SSToDS_16a endp

CODE16 ends


;
; all segments have a <segmentname>START label at the start of the segment.
;

CODE32 segment
CODE32START label byte

;;
; Gets the current cs.
; @cproto    none.
; @returns   CS
; @author    knut st. osmundsen
; @remark    internal method. called from 16-bit code...
far_getCS proc far
    ASSUME DS:nothing, ES:nothing
    mov     ax,  cs
    retf
far_getCS endp



;;
; Gets the a 32-bit flat pointer to the OS/2 Kernel MTE.
; @cproto    extern PMTE _System GetOS2KrnlMTE(void);
; @returns   Pointer to kernel MTE.
; @status    completely implemented.
; @author    knut st. osmundsen
GetOS2KrnlMTE PROC NEAR
    push    es

    mov     ax,  SAS_selector               ;70h - Read-only SAS selector.
    mov     es,  ax
    xor     ebx, ebx
    assume  ebx: PTR SAS
    mov     bx,  es:[ebx].SAS_vm_data       ;SAS_vm_data (0ch)
    assume  ebx: PTR SAS_vm_section
    mov     eax, es:[ebx].SAS_vm_krnl_mte   ;SAS_vm_krnl_mte (0ch)

    pop     es
    ret
GetOS2KrnlMTE ENDP

CODE32 ends


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

_VFT segment
_VFTSTART LABEL BYTE
_VFT ends

EH_DATA segment
EH_DATASTART LABEL BYTE
EH_DATA ends

END

