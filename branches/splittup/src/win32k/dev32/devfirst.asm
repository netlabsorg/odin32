; $Id: devfirst.asm,v 1.9.2.1 2001-09-27 03:08:14 bird Exp $
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
    public CODE16_INITSTART
    public DATA16START
    public DATA16START
    public DATA16_BSSSTART
    public DATA16_CONSTSTART
    public DATA16_INITSTART
    public CODE16START
    public CODE32START
    public DATA32START
    public BSS32START
    public CONST32_ROSTART
    public _VFTSTART
    public EH_DATASTART

    public _strategyAsm
    public _CallWin32kIOCtl
    public _CallWin32kOpen
    public _CallWin32kClose
    public _SSToDS_16a


;
; Externs
;
    extrn _TKSSBase:dword
    extrn WIN32KIOCTL:FAR
    extrn WIN32KOPEN:FAR
    extrn WIN32KCLOSE:FAR
    .286p
    extrn h_POST_SIGNAL:FAR
    extrn _strategy:near


CODE16 segment
    ASSUME CS:CODE16, DS:DATA16, ES:NOTHING, SS:NOTHING

CODE16START db 'CODE16START',0


    .286p
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
; Thunk procedure for .
; @cproto    USHORT NEAR CallWin32kOpen(LIN);
; @returns   Same as Win32kOpen
; @param     address of Open request packet (32-bit pointer).
; @status    completely implemented.
; @author    knut st. osmundsen
_CallWin32kOpen PROC NEAR
    ASSUME CS:CODE16
    push    ds
    push    word ptr [esp+6]            ; push high word.
    push    word ptr [esp+6]            ; push low word.
    call    far ptr FLAT:WIN32KOPEN
    pop     ds
    retn
_CallWin32kOpen ENDP


;;
; Thunk procedure for strategy close.
; @cproto    USHORT NEAR CallWin32kClose(LIN);
; @returns   Same as Win32kIOCtl
; @param     address of Open request packet (32-bit pointer).
; @status    completely implemented.
; @author    knut st. osmundsen
_CallWin32kClose PROC NEAR
    ASSUME CS:CODE16
    push    ds
    push    word ptr [esp+6]            ; push high word.
    push    word ptr [esp+6]            ; push low word.
    call    far ptr FLAT:WIN32KCLOSE
    pop     ds
    retn
_CallWin32kClose ENDP


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
_SSToDS_16a proc NEAR
    assume CS:CODE16, DS:DATA16, ES:NOTHING
    call    far ptr FLAT:far_getCS      ; get flat selector.
    push    es
    mov     es,  ax
    assume  es:FLAT
    mov     eax, es:_TKSSBase           ; get pointer to stack base
    pop     es
    movzx   edx, word ptr ss:[esp + 2]  ; 16-bit stack pointer (parameter)
    add     eax, edx                    ; 32-bit stack pointer in eax
    mov     edx, eax
    shr     edx, 16                     ; dx high 16-bit of 32-bit stack pointer.
    retn
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


;;
; Post signal to one or more processes.
; @cproto   extern ULONG POST_SIGNAL32(USHORT usSignal, USHORT usAction, USHORT usSignalArg, USHORT usPIDSGR);
; @returns  NO_ERROR on success.
;           On error ERROR_NOT_DESCENDANT, ERROR_SIGNAL_REFUSED,
;           ERROR_INVALID_PROCID, ERROR_ZOMBIE_PROCESS, ERROR_SIGNAL_PENDING. (it seems)
; @param    ax          Signal number.
; @param    dx          Action.
;                       0 - the process and all children.
;                       1 - only the process
;                       2 - the process and all it's decendants.
;                       3 - all processes in that screen group.
; @param    cx          Signal argument.
; @param    [ebp+14h]   Process Id or Screen Group Id.
; @uses     eax, edx, ecx
; @status   completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark   Used by importTabInit.
POST_SIGNAL32 proc near
    push    ebp
    mov     ebp, esp
    push    ebx

    movzx   eax, ax                     ; signal
    movzx   ebx, dx                     ; action
    movzx   ecx, cx                     ; argument
    movzx   edx, word ptr [ebp+14h]     ; pid

    jmp     far ptr CODE16:Thunk16_POST_SIGNAL32
Thunk32_POST_SIGNAL32::
    jc      cps_error                   ; jump on error
    xor     eax, eax                    ; just to make sure it's all empty.
    jmp     cps_end

cps_error:
    movzx   eax, ax                     ; make sure upper part is empty as well.

cps_end:
    pop     ebx
    leave
    ret
POST_SIGNAL32 endp
CODE32 ends

CODE16 segment
Thunk16_POST_SIGNAL32::
    call    far ptr h_POST_SIGNAL
    jmp     far ptr FLAT:Thunk32_POST_SIGNAL32
CODE16 ends




CODE16_INIT segment
CODE16_INITSTART db 'CODE16_INITSTART',0
CODE16_INIT ends

DATA16 segment
DATA16START label byte ; no string here!!
DATA16 ends

DATA16_BSS segment
DATA16_BSSSTART db 'DATA16_BSSSTART',0
DATA16_BSS ends

DATA16_CONST segment
DATA16_CONSTSTART db 'DATA16_CONSTSTART', 0
DATA16_CONST ends

DATA16_INIT segment
DATA16_INITSTART db 'DATA16_INITSTART',0
DATA16_INIT ends

DATA32 segment
DATA32START db 'DATA32START',0
DATA32 ends

BSS32 segment
BSS32START db 'BSS32START',0
BSS32 ends

CONST32_RO segment
CONST32_ROSTART db 'CONST32_ROSTART',0
CONST32_RO ends

_VFT segment
_VFTSTART db '_VFTSTART',0
_VFT ends

EH_DATA segment
EH_DATASTART db 'EH_DATASTART',0
EH_DATA ends

END

