; $Id: DH.asm,v 1.2 2001-09-17 02:27:55 bird Exp $
;
; Device Helper Router.
; Stack Thunker.
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386


;
; Include files
;
    include devsegdf.inc
    include devhlp.inc


;
; Exported symbols
;
    public  ThunkStack16To32
    public  ThunkStack32To16
    public  TKSSBase
    public  getDHRouterFarPtr
    ;public  DHRouter


;
; extrns
;
    extrn DOS32ALLOCMEM:PROC
    extrn DOS32FREEMEM:PROC
    extrn DOS16GETINFOSEG:PROC
    extrn _Device_Help:DWORD
    extrn pulTKSSBase32:DWORD
    extrn _TKSSBase16:DWORD
    extrn _R0FlatCS16:WORD
    extrn _R0FlatDS16:WORD




CODE16 SEGMENT
    ASSUME cs:CODE16, ds:FLAT



;;
; Devhelp routiner routine.
; @cproto none
; @param     dl     Devhelper routine.
; @uses      none but the ones returned.
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
DHRouter PROC FAR
    ASSUME cs:CODE16, ds:NOTHING, es:NOTHING, ss:NOTHING
    ;
    ; Preserve nearly everything (as a start)
    ;
    push    ds
    push    es
    push    fs
    push    gs
    push    edi
    push    esi
    push    ebp
    push    edx
    push    ecx
    push    ebx

    cmp     dl, DevHlp_VirtToLin
    jne     dhr1
    call    dh_VirtToLin
    jmp     dhr_ret

dhr1:
    cmp     dl, DevHlp_VMAlloc
    jne     dhr2
    call    dh_VMAlloc
    jmp     dhr_ret

dhr2:
    cmp     dl, DevHlp_VMFree
    jne     dhr3
    call    dh_VMFree
    jmp     dhr_ret

dhr3:
    cmp     dl, DevHlp_GetDOSVar
    jne     dhr4
    call    dh_GetDOSVar
    add     sp, 4                       ; remove ebx from stack
    jmp     dhr_ret_ebx

dhr4:
    cmp     dl, DevHlp_VMLock
    jne     dhr5
    call    dh_VMLock
    jmp     dhr_ret

dhr5:
    cmp     dl, DevHlp_VMUnlock
    jne     dhr6
    call    dh_VMUnlock
    jmp     dhr_ret

dhr6:
    cmp     dl, DevHlp_VerifyAccess
    jne     dhr7
    call    dh_VerifyAccess
    jmp     dhr_ret

dhr7:

dhr_notimplemented:
    stc
    mov     eax, 87 ;ERROR_INVALID_PARAMETER

    ;
    ; Restore nearly everything (as a start)
    ;
dhr_ret:
    pop     ebx
dhr_ret_ebx:
    pop     ecx
    pop     edx
    pop     ebp
    pop     esi
    pop     edi
    pop     gs
    pop     fs
    pop     es
    pop     ds
    ret
DHRouter ENDP





;;
; Converts a virtual (sel:off) address to a linear address.
; @cproto    none
; @param     AX     Selector
; @param     ESI    Offset
; @param     DL     DevHelp_VirtToLin
; @returns   Success:
;               CF      clear
;               EAX     Linear address
;
;            Failure:
;               CF      set
;               EAX     Error code.
;
; @uses      eax
; @status    completely impelemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
dh_VirtToLin PROC NEAR
    ASSUME cs:CODE16, ds:NOTHING, es:NOTHING, ss:NOTHING
    shl     eax, SEL_FLAT_SHIFT
    and     eax, SEL_FLATMASK
    add     eax, esi
vtl_retok:
    clc
    ret

vtl_reterr:
    stc
    ret
dh_VirtToLin ENDP




;;
; Allocates virtual memory.
; @cproto    none
; @param     ECX    Amount of memory to allocate.
; @param     EDI    Pointer to DD var containing physical address to be mapped.
;                   -1 if not used.
; @param     EAX    Flags. These flags are supported:
;                       VMDHA_SWAP
;                       VMDHA_USEHIGHMEM
;                       VMDHA_RESERVE
; @param     DH     DevHlp_VMAlloc
;
; @returns   Success:
;               CF      Clear
;               EAX     Linear address of object.
;
;            Failure:
;               CF      Set.
;               EAX     Error code. ERROR_INVALID_PARAMETER.
;
; @uses      edx, ecx, eax
; @equiv
; @time
; @sketch    For it into an call to DosAllocMem
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
dh_VMAlloc PROC NEAR
    ASSUME cs:CODE16, ds:NOTHING, es:NOTHING, ss:NOTHING

; define the flags which are used here
VMDHA_SWAP          EQU 0004h
VMDHA_RESERVE       EQU 0100h
VMDHA_USEHIGHMEM    EQU 0800h

    ;
    ; Check input parametes.
    ;
    mov     edx, eax
    and     edx, NOT (VMDHA_SWAP OR VMDHA_USEHIGHMEM OR VMDHA_RESERVE)
    jnz     vma_reterr                  ; Jump if unsupported flag(s) are set.
    cmp     edi, 0ffffffffh             ; Check that EDI is -1.
    jne     vma_reterr                  ; Jump if EDI != -1

    jmp     far ptr FLAT:Thunk32_dh_VMAlloc
Thunk16_dh_VMAlloc::
    or      eax, eax                    ; Successful return from Dos32AllocMem?
    jnz     vma_reterr                  ; Jump on failure.
    mov     eax, edx                    ; EAX is now 32-bit pointer to the allocated memory.

vma_retok:
    clc
    ret

vma_reterr:
    mov     eax, 87                     ; ERROR_INVALID_PARAMETER
    stc
    ret
dh_VMAlloc ENDP
CODE16 ENDS


CODE32 SEGMENT
Thunk32_dh_VMAlloc::
    ASSUME cs:CODE32, ds:NOTHING, es:NOTHING, ss:NOTHING
    ;
    ; Call Dos32AllocMem to allocate the requested memory.
    ;
    push    eax                         ; Make space for return pointer.
    mov     edx, esp                    ; Take address of return pointer.
    test    edx, 0ffff0000h
    jnz     vma_stack32                 ; Jump if stack is 32-bit
    xor     edx, edx
    mov     dx, ss                      ; Make 32-bit pointer to return pointer.
    shl     edx, SEL_FLAT_SHIFT
    and     edx, SEL_FLATMASK
    mov     dx,  sp                     ; EDX is now 32-bit pointer to return pointer.
vma_stack32:

    test    eax, VMDHA_RESERVE
    jz      vma1
    push    0043h                       ; Flags: OBJ_TILE | PAG_READ | PAG_WRITE
    jmp     vma2
vma1:
    push    0053h                       ; Flags: OBJ_TILE | PAG_COMMIT | PAG_READ | PAG_WRITE
vma2:
    push    ecx                         ; Number of bytes to allocate.
    push    edx                         ; Pointer to return pointer.
    call    DOS32ALLOCMEM
    add     esp, 12
    pop     edx                         ; Pointer to allocated memory if any...

    jmp     far ptr CODE16:Thunk16_dh_VMAlloc
CODE32 ENDS


CODE16 SEGMENT
;;
; Frees memory allocated with VMAlloc.
; @cproto    none
; @param     EAX        Linear address of the memory to be freed.
; @returns   Success:
;               CF      Clear
;
;            Failure:
;               CF      Set
;               EAX     Errorcode
;                       ERROR_ACCESS_DENIED
;                       ERROR_INVALID_PARAMETER
;
; @uses      eax, ecx, edx
; @status    completely implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
dh_VMFree PROC NEAR
    ASSUME cs:CODE16, ds:NOTHING, es:NOTHING, ss:NOTHING
    jmp     far ptr FLAT:Thunk32_dh_VMFree
Thunk16_dh_VMFree::
    or      eax, eax
    jnz     vmf_reterr

vmf_retok:
    clc
    ret

vmf_reterr:
    stc
    ret
dh_VMFree ENDP
CODE16 ENDS


CODE32 SEGMENT
; 32-bit part of the dh_VMFree worker
Thunk32_dh_VMFree::
    ASSUME cs:CODE32, ds:NOTHING, es:NOTHING, ss:NOTHING
    push    eax
    call    DOS32FREEMEM
    sub     esp, 4
    jmp     far ptr CODE16:Thunk16_dh_VMFree
CODE32 ENDS


CODE16 SEGMENT
;;
; Get addresses of kernel variables.
; @cproto    none.
; @param     AL     Index.
;                   These indexes are supported:
;                       DHGETDOSV_SYSINFOSEG     1
;                       DHGETDOSV_LOCINFOSEG     2
;                       DHGETDOSV_YIELDFLAG      7
;                       DHGETDOSV_TCYIELDFLAG    8
;                       DHGETDOSV_DOSTABLE       9
;
;
; @param     CX     VarMember (only index 14 and 16)
; @param     DL     DevHlp_GetDOSVar
; @returns   Success:
;               CF      Clear
;               AX:BX   Points to the index.
;
;            Failure:
;               CF      Set.
;
; @uses     eax, ebx, ecx, edx
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
dh_GetDOSVar PROC NEAR
    ASSUME cs:CODE16, ds:NOTHING, es:NOTHING, ss:NOTHING
    cmp     al, 1
    je      gdv_12
    cmp     al, 2
    je      gdv_12
    cmp     al, 7
    je      gdv_8
    cmp     al, 8
    je      gdv_8
    cmp     al, 9
    je      gdv_9
    jmp     gdv_reterr

gdv_12:
    push    ax
    push    ax                          ; make space for selGIS
    mov     ax, sp                      ; offset selGIS
    push    ax                          ; make space for selLIS
    mov     bx, sp                      ; offset selLIS
    push    ss
    push    ax                          ; &selGIS
    push    ss
    push    bx                          ; &selLIS
    call    far ptr DOS16GETINFOSEG
    pop     bx                          ; selLIS
    pop     cx                          ; selGIS
    pop     dx                          ; index parameter
    or      ax, ax                      ; Did it fail?
    jnz     gdv_reterr                  ; Jump if it failed.
    cmp     dx, 1
    je      gdv_12_1
    mov     bx, cx                      ; bx <- selGIS
gdv_12_1:
    xor     ax, ax
    jmp     gdv_retok

gdv_7:
    mov     ebx, offset YieldFlag
    mov     ax, seg YieldFlag
    jmp     gdv_retok

gdv_8:
    mov     ebx, offset TCYieldFlag
    mov     ax, seg TCYieldFlag
    jmp     gdv_retok

gdv_9:
    mov     ebx, offset DosTable
    mov     ax, seg DosTable
    jmp     gdv_retok

gdv_retok:
    clc
    ret

gdv_reterr:
    stc
    ret
dh_GetDOSVar ENDP
CODE16 ENDS


DATA16 SEGMENT
;; GetDosVar data
;
; The two YieldFlags
;
YieldFlag                       db 0
TCYieldFlag                     db 0
DATA16 ENDS


DATA32 SEGMENT
;
; 32-bit flat stack base.
;
TKSSBase                        dd 0
DATA32 ENDS

DATA16 SEGMENT
    ASSUME ds:NOTHING, es:NOTHING, ss:NOTHING
;
; The two dostables.
;
DosTable                        db 17
  DosTable_fph_HardError        dd 0
  DosTable_fph_UCase            dd 0
  DosTable_UnknownOrReserved1   dd 0
  DosTable_UnknownOrReserved2   dd 0
  DosTable_UnknownOrReserved3   dd 0
  DosTable_fph_MemMapAlias      dd 0
  DosTable_fph_MemUnmapAlias    dd 0
  DosTable_fph_GoProtAll        dd 0
  DosTable_fph_GoRealAll        dd 0
  DosTable_fph_doshlp_RedirDev  dd 0
  DosTable_UnknownOrReserved4   dd 0
  DosTable_UnknownOrReserved5   dd 0
  DosTable_fph_SFFromSFN        dd 0
  DosTable_fph_SegGetInfo       dd 0
  DosTable_fph_AsgCheckDrive    dd 0
  DosTable_UnknownOrReserved6   dd 0
  DosTable_UnknownOrReserved7   dd 0

DosTabel2                       db  19
  DosTable2_fpErrMap24          dd 0
  DosTable2_fpErrMap24End       dd 0
  DosTable2_fpErr_Table_24      dd 0
  DosTable2_fpCDSAddr           dd 0
  DosTable2_fpGDT_RDR1          dd 0
  DosTable2_fpInterrupLevel     dd 0
  DosTable2_fp_cInDos           dd 0
  DosTable2_UnknownOrReserved1  dd 0
  DosTable2_UnknownOrReserved2  dd 0
  DosTable2_R0FlatCS            dw seg FLAT:CODE32
                                dw 0
  DosTable2_R0FlatDS            dw seg FLAT:DATA32
                                dw 0
  DosTable2_pTKSSBase           dd offset FLAT:DATA32:TKSSBase
  DosTable2_pintSwitchStack     dd 0
  DosTable2_pprivatStack        dd 0
  DosTable2_fpPhysDiskTablePtr  dd 0
  DosTable2_pforceEMHandler     dd 0
  DosTable2_pReserveVM          dd 0
  DosTable2_p_pgpPageDir        dd 0
  DosTable2_UnknownOrReserved3  dd 0
DATA16 ENDS


CODE16 SEGMENT


;;
; VMLock stub.
; @returns  NO_ERROR. (0)
; @param    ignored.
; @uses     eax
; @status   stub.
; @author   knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
dh_VMLock PROC NEAR
    xor     eax, eax
    clc
    ret
dh_VMLock ENDP



;;
; VMUnlock stub.
; @returns  NO_ERROR. (0)
; @param    ignored.
; @uses     eax
; @status   stub.
; @author   knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
dh_VMUnlock PROC NEAR
    xor     eax, eax
    clc
    ret
dh_VMUnlock ENDP


;;
; VerifyAccess stub.
; @returns  NO_ERROR. (0)
; @param    ignored.
; @uses     eax
; @status   stub.
; @author   knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
dh_VerifyAccess PROC NEAR
    xor     eax, eax
    clc
    ret
dh_VerifyAccess ENDP


CODE16 ENDS




CODE32 SEGMENT
;;
; Initiates the Device_Help function pointer.
;
; During startup the C library startup routines needs to use the heap.
; So the heap has to work before main is able to initiate it. The heap
; uses device helpers so they have to be initiated too. So, this init
; routine is called during heap init, _rmem_init.
;
; @cproto    void _Optlink dhinit(void)
; @returns   NO_ERROR.
; @param     none.
; @uses      eax
; @sketch
; @status    completly implemented
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
dhinit PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING
    ;
    ; _Device_Help - d16globl.c
    ;
if 0 ;done during device init.
    mov     ax, seg DHRouter
    shl     eax, 16
    mov     ax, offset DHRouter
    mov     _Device_Help, eax
endif

    ;
    ; pulTKSSBase32
    ; DosTable2_pTKSSBase
    ; DosTable2_R0FlatCS
    ; DosTable2_R0FlatDS
    ;
    mov     eax, offset FLAT:TKSSBase
    mov     pulTKSSBase32, eax          ; We need it as our testing subsystem uses it too.
if 0 ; not neede any longer
    ;mov     DosTable2_pTKSSBase, eax
    ;mov     _TKSSBase16, eax

    ;xor     eax, eax
    ;mov     ax, seg FLAT:CODE32
    ;mov     DosTable2_R0FlatCS, ax
    ;mov     _R0FlatCS16, ax

    ;mov     ax, seg FLAT:DATA32
    ;mov     DosTable2_R0FlatDS, ax
    ;mov     _R0FlatDS16, ax
endif

    ;
    ; return NO_ERROR
    ;
    xor     eax, eax
    ret
dhinit ENDP


;;
; Gets the a 16-bit far pointer to the DHRouter routine.
; @cproto    ULONG  _Optlink getDHRouterFarPtr(VOID); /* dh.asm */
; @returns   16-bit far pointer to the DHRouter routine.
; @uses      EAX
; @status    completely implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
getDHRouterFarPtr PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING
    mov     ax, seg CODE16:DHRouter
    shl     eax, 16
    mov     ax, offset CODE16:DHRouter
    ret
getDHRouterFarPtr ENDP





;;
; Thunks the stack from 16-bits to 32-bits.
; !The stack has to be 16-bits on entry!
; @cproto    VOID _Optlink ThunkStack16To32(VOID)
; @returns   void
;            Stack is 32-bits!
;            EBP and SS:ESP is thunked
;
; @uses      EAX, EDX
; @sketch
; @status    completly implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    RING 3 only.
ThunkStack16To32 PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING
    movzx   esp, sp                     ; Make sure high end of esp is zero.
    mov     ax, ss                      ; Find the linary base of the segment addressed by ss.
    shl     eax, SEL_FLAT_SHIFT
    and     eax, SEL_FLATMASK
    mov     ax, sp                      ; Add the segment offset, which is sp.
                                        ; eax is now the linear stack address equal to ss:sp.

    mov     dx, seg FLAT:DATA32         ; ALP bug? Can't: push seg FLAT:DATA32
                                        ; Have to move it to dx before pushing it.
    push    dx                          ; Make lss quad word: new ss, new esp
    push    eax
    lss     esp, ss:[esp]               ; load new ss and esp values.

    ;
    ; Fix ebp
    ; Creates new ebp from high word of esp and low word of ebp.
    ;
    mov     eax, esp
    mov     ax, bp
    mov     ebp, eax

    ;
    ; Update TKSSBase
    ;
    push    ds
    mov     ds, dx
    ASSUME  ds:FLAT
    mov     TKSSBase, 0
    pop     ds
    ret
ThunkStack16To32 ENDP


;;
; Thunks the stack from 32-bits to 16-bits.
; !The stack has to be 32-bits on entry!
; @cproto    VOID _Optlink ThunkStack32To16(VOID)
; @returns   void
;            Stack is 16-bits!
;            EBP and SS:ESP is thunked
;
; @uses      EAX, EDX
; @sketch
; @status    completly implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    RING 3 only.
ThunkStack32To16 PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING

    mov     edx, esp                    ; save esp for later use (setting TKSSBase).

    ;
    ; calc LDT selector from linear esp and load new ss:esp.
    ;
    mov     eax, esp
    shr     eax, SEL_FLAT_SHIFT
    or      ax,  SEL_LDT_RPL3           ; ax new ss LDT (RING3).
    rol     eax, 16
    mov     ax, sp
    push    eax
    lss     sp, dword ptr [esp]         ; load ss:sp.
    movzx   esp, sp                     ; Zero upper part of esp.
                                        ; Stack is now 16 bits
    ;
    ; Fix ebp
    ;
    movzx   ebp, bp                     ; Zero upper part of ebp.

    ;
    ; Update TKSSBase
    ;
    push    ds
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    ASSUME  ds:FLAT
    xor     dx, dx                      ; zero lower word of dx to get linear
                                        ; address of 16-bits stack base
    mov     TKSSBase, edx
    pop     ds

    ret
ThunkStack32To16 ENDP


CODE32 ENDS

end

