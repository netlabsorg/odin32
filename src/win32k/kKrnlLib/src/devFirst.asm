; $Id: devFirst.asm,v 1.4 2002-12-19 01:49:07 bird Exp $
;
; DevFirst - entrypoint, helper code, and segment definitions.
;
;
; Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
;
; This file is part of kKrnlLib.
;
; kKrnlLib is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; kKrnlLib is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with kKrnlLib; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;
    .386p

;
; Include files
;
    include devsegdf.inc
    include devhdr.inc
    include sas.inc

;
; Exported symbols
;
    public CODE16START
    public DATA16START
    public DATA16_BSSSTART
    public DATA16_CONSTSTART
    public DATA16_GLOBALSTART
    public CODE16START
    public CODE32START
    public DATA32START
    public BSS32START
    public CONST32_ROSTART
    public _VFTSTART
    public EH_DATASTART


    public _strategyAsm0
    public _strategyAsm1
    public _CallR0Addr32bit
    public _SSToDS_16a
    public krnlGetOS2KrnlMTE
    public krnlMakeCalltab16CodeSegment
    public x86DisableWriteProtect
    public x86RestoreWriteProtect
    public _GetR0InitPtr
    public _GetVerifyImportTab32Ptr
    public _GetGetKernelInfo32Ptr
    public _GetLookupKrnlEntry32Ptr
    public _DATA16Base


;
; Externs
;
DATA32 segment
    extrn _pulTKSSBase32:dword
DATA32 ends
    extrn R0Init:near
    extrn VerifyImportTab32:near
    extrn GetKernelInfo32:near
    extrn LookupKrnlEntry32:near
    extrn R0Addr32bit:near
    .286p
    extrn _strategy:near

CODE16 segment
    ASSUME CS:CODE16, DS:DATA16, ES:NOTHING, SS:NOTHING

CODE16START db 'CODE16START',0

    .286p
;$win32ki entry point
_strategyAsm0:
;    int 3
    push    0
    jmp     _strategyAsm

;$win32k entry point
_strategyAsm1:
;    int 3
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
;            Not callable from Ring-3 init.
_SSToDS_16a proc NEAR
    assume CS:CODE16, DS:DATA16, ES:NOTHING
    mov     edx, ds:_pulTKSSBase32      ; get pointer held by _pulTKSSBase32 (pointer to stack base)
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
    retn
_SSToDS_16a endp

;;
; Thunk procedure for IOCtl intended to call R0 init
; proc in other drivers.
; @cproto    USHORT NEAR CallR0Addr32bit(LIN ulAddr, ULONG ulParam);
; @returns   Same as R0Addr32bit.
; @param     ulAddr     32-bit address which we will call to.
; @status    completely implemented.
; @author    knut st. osmundsen
_CallR0Addr32bit PROC NEAR
    ASSUME CS:CODE16
    call    far ptr FLAT:ASMR0Addr32bit
    ret 4
_CallR0Addr32bit ENDP



;;
; Gets the 32-bit flat pointer of R0Init. (32-bit init function)
; @cproto   ULONG GetR0InitPtr(void);
; @returns  Flat pointer to R0Init. (ax:dx)
; @uses     uses eax, edx.
; @author   knut st. osmundsen (kosmunds@csc.com)
_GetR0InitPtr PROC NEAR
    mov     eax, offset FLAT:CODE32:R0Init
    mov     edx, eax
    shr     edx, 16
    ret
_GetR0InitPtr ENDP


;;
; Gets the 32-bit flat pointer of VerifyImportTab32. (32-bit verify function)
; @cproto   ULONG GetVerifyImportTab32Ptr(void);
; @returns  Flat pointer to VerifyImportTab32. (ax:dx)
; @uses     uses eax, edx.
; @author   knut st. osmundsen (kosmunds@csc.com)
_GetVerifyImportTab32Ptr PROC NEAR
    mov     eax, offset FLAT:CODE32:VerifyImportTab32
    mov     edx, eax
    shr     edx, 16
    ret
_GetVerifyImportTab32Ptr ENDP


;;
; Gets the 32-bit flat pointer of GetKernelInfo32.
; @cproto   ULONG GetGetKernelInfo32Ptr(void);
; @returns  Flat pointer to GetKernelInfo32. (ax:dx)
; @uses     uses eax, edx.
; @author   knut st. osmundsen (kosmunds@csc.com)
_GetGetKernelInfo32Ptr PROC NEAR
    mov     eax, offset FLAT:CODE32:GetKernelInfo32
    mov     edx, eax
    shr     edx, 16
    ret
_GetGetKernelInfo32Ptr ENDP


;;
; Gets the 32-bit flat pointer of LookupKrnlEntry32.
; @cproto   ULONG GetLookupKrnlEntry32Ptr(void);
; @returns  Flat pointer to LookupKrnlEntry32. (ax:dx)
; @uses     uses eax, edx.
; @author   knut st. osmundsen (kosmunds@csc.com)
_GetLookupKrnlEntry32Ptr PROC NEAR
    mov     eax, offset FLAT:CODE32:LookupKrnlEntry32
    mov     edx, eax
    shr     edx, 16
    ret
_GetLookupKrnlEntry32Ptr ENDP

CODE16 ends


;
; all segments have a <segmentname>START label at the start of the segment.
;

CODE32 segment
CODE32START db 'CODE32START',0


;;
; Worker for _CallR0Addr32bit. Calls to 32-bit routine.
; We're calling the function using the _System callingconvention.
ASMR0Addr32bit proc far
    ASSUME DS:nothing, ES:nothing
    ;
    ; Read parameters from _CallR0Addr32bits call frame before we mess with esp.
    ;
    mov     edx, [esp+0ah]              ; 32-bit routine address
    mov     eax, [esp+0eh]              ; Parameter

    ; save most things. (paranoia minor)
    push    ds
    push    es
    push    fs
    push    gs
    push    ebx
    push    esi
    push    edi

    ; load new segment registers.
    mov     cx, seg FLAT:DATA32
    mov     ds, cx
    mov     es, cx
    xor     ecx, ecx
    mov     fs, cx      ;?
    ASSUME  ds:FLAT, es:FLAT, fs:NOTHING

    ; do the call. (_System or _Optlink)
if 0                                    ; of logging reasons we'll call thru a c function.
;    push    eax
;    call    edx we'll call
;    add     esp, 4
else
     push   edx
     push   eax
     call   R0Addr32bit
     add    esp, 8
endif

    ; restore most things. (paranoia minor)
    pop     edi
    pop     esi
    pop     ebx
    pop     gs
    pop     fs
    pop     es
    pop     ds

    ;
    ; Return ulong using ax:dx.
    ;
    mov     edx, eax
    shr     edx, 16
    retf
ASMR0Addr32bit endp


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
; @cproto    extern PMTE _System krnlGetOS2KrnlMTE(void);
; @returns   Pointer to kernel MTE.
; @status    completely implemented.
; @author    knut st. osmundsen
krnlGetOS2KrnlMTE PROC NEAR
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
krnlGetOS2KrnlMTE ENDP


;;
; Disables the ring-0 write protection.
; It's used to help us write to readonly code segments and objects.
; @cproto   extern ULONG    _Optlink x86DisableWriteProtect(void);
; @return   Previous write protection flag setting.
; @uses     eax, edx
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark   Used by importTabInit.
x86DisableWriteProtect proc near
    cli
    mov     edx, cr0                    ; Get current cr0
    test    edx, 000010000h             ; Test for the WriteProtect flag (bit 16)
    setnz   al
    movzx   eax, al                     ; Old flag setting in eax (return value)
    and     edx, 0fffeffffh             ; Clear the 16th (WP) bit.
    mov     cr0, edx                    ;
    sti
    ret                                 ; return eax holds previous WP value.
x86DisableWriteProtect endp


;;
; Restore the WP flag of CR0 to it's previous state.
; The call is intent only to be called with the result from x86DisableWriteProtect,
; and will hence only enable the WP flag.
; @cproto   extern ULONG    _Optlink x86RestoreWriteProtect(ULONG flWP);
; @return   Previous write protection flag setting.
; @param    eax - flWP  Boolean value. (1 = WP was set, 0 WP was clear)
; @uses     eax
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark   Used by importTabInit.
x86RestoreWriteProtect proc near
    test    eax, eax                    ; Check if the flag was previously clear
    jnz     x86RWP_set                  ; If set Then Set it back.
    jmp     x86RWP_end                  ; If clear Then nothing to do.
x86RWP_set:
    cli
    mov     eax, cr0                    ; Get current cr0.
    or      eax, 000010000h             ; The the 16-bit (WP) bit.
    mov     cr0, eax                    ; Update cr0.
    sti

x86RWP_end:
    ret
x86RestoreWriteProtect endp


CODE32 ends


DATA32 segment
DATA32START db 'DATA32START',0
GDTR_limit      dw ?                    ; The limit field of the GDTR.
GDTR_base       dd ?                    ; The base field of the GDTR. (linear flat address)
DATA32 ends

CODE32 segment


;;
; Changes the calltab16 data segment to a code segment.
; @cproto   int _Optlink MakeCalltab16CodeSegment(void);
; @returns  0 on success.
;           -1 on error.
; @uses     eax
; @author   knut st. osmundsen (kosmunds@csc.com)
krnlMakeCalltab16CodeSegment proc near
    ASSUME  ds:FLAT, ss:nothing
    xor     eax, eax
    mov     ax, seg CALLTAB16
    and     ax, 0fff8h                  ; clear the dpl bits and descriptor type bit. (paranoia!)
    sgdt    GDTR_limit
    cmp     ax, GDTR_limit              ; check limit. (paranoia!!!)
    jl      mccs_OK
    xor     eax, eax
    dec     eax                         ; eax = -1
    jmp     mccs_ret

mccs_OK:
    add     eax, GDTR_base              ; GDTR_base + selector offset -> flat pointer to selector.

    ;
    ; eax is pointing to the descriptor table entry for my GDT selector.
    ; Now we'll have to change it into a callgate.
    ; This is the layout of a callgate descriptor:
    ;  bits
    ;  0-15         Segment limit
    ; 16-31         Segment base
    ; -------second dword-------
    ; 32-39  0-7    Segment base (cont.)
    ; 40-43  8-11   Selector type
    ;    44  12     Segment type. 0=system, 1=application
    ; 45-46  13-14  Descriptor Privelege Level (DPL)
    ;    47  15     Present flag.
    ; 48-51  16-19  Segment limit (cont.)
    ;    52  20     UVirt flag (OS2)
    ;    53  21     Reserved (AMD 64-bit segment flag). Zero.
    ;    54  22     Default address operand.
    ;    55  23     Granularity (0=bytes, 1=pages)
    ; 56-63  Base (cont.)
    ;
    ; So what we have to do is to change the type from DATA to CODE.
    ; selector type:
    ;    43  11     0 = data, 1 = code
    ; Data:
    ;    41  9      Writable:   0=readonly 1=read/write
    ;    42  10     Expansion:  0=expandup 1=expanddown
    ; Code:
    ;    41  9      Readable:   0=executeonly   1=read/execute
    ;    42  10     Conforming: 0=nonconforming 1=conforming
    ;
    or      word ptr [eax + 4], 00a00h  ; code and read/execute
    and     word ptr [eax + 4], 0fbffh  ; nonconfiming.

    xor     eax, eax                    ; success.
mccs_ret:
    ret
krnlMakeCalltab16CodeSegment endp


CODE32 ends


DATA16 segment
DATA16START label byte                  ; Note. no start string here!

aDevHdrs: ;DDHDR aDevHdrs[2] /* This is the first piece data in the driver!!!!!!! */
;
; device 1
;
    dw size SysDev3                     ; NextHeader (offset)
    dw seg aDevHdrs                     ; NextHeader (selector)
    dw DEVLEV_3 or DEV_30 or DEV_CHAR_DEV ; SDevAtt
    dw offset _strategyAsm0             ; StrategyEP
    dw 0                                ; InterruptEP
    db "$KrnlHlp"                       ; DevName
    dw 0                                ; SDevProtCS
    dw 0                                ; SDevProtDS
    dw 0                                ; SDevRealCS
    dw 0                                ; SDevRealDS
    dd DEV_16MB or DEV_IOCTL2           ; SDevCaps
;
; device 2
;
    dw 0ffffh                           ; NextHeader (offset)
    dw 0ffffh                           ; NextHeader (selector)
    dw DEVLEV_3 or DEV_30 or DEV_CHAR_DEV ; SDevAtt
    dw offset _strategyAsm1             ; StrategyEP
    dw 0                                ; InterruptEP
    db "$KrnlLib"                       ; DevName
    dw 0                                ; SDevProtCS
    dw 0                                ; SDevProtDS
    dw 0                                ; SDevRealCS
    dw 0                                ; SDevRealDS
    dd DEV_16MB or DEV_IOCTL2           ; SDevCaps
DATA16 ends

DATA16_BSS segment
DATA16_BSSSTART db 'DATA16_BSSSTART',0
DATA16_BSS ends

DATA16_CONST segment
DATA16_CONSTSTART db 'DATA16_CONSTSTART',0
DATA16_CONST ends

DATA16_GLOBAL segment
DATA16_GLOBALSTART db 'DATA16_GLOBALSTART',0
_DATA16Base dd offset FLAT:DATA16:DATA16START
DATA16_GLOBAL ends

BSS32 segment
BSS32START db 'BSS32START',0
BSS32 ends

CONST32_RO segment
CONST32_ROSTART db 'CONST32_ROSTART', 0
CONST32_RO ends

_VFT segment
_VFTSTART db '_VFTSTART', 0
_VFT ends

EH_DATA segment
EH_DATASTART db 'EH_DATASTART', 0
EH_DATA ends

END

