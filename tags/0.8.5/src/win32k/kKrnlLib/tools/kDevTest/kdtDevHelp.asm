; $Id: kdtDevHelp.asm,v 1.3 2002-10-14 15:17:05 bird Exp $
;
; Device Helper Implmenetation for Ring-3 testing.
;
; Copyright (c) 2000-2002 knut st. osmundsen <bird@anduin.net>
;
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

    .386


;*******************************************************************************
;* Header Files                                                                *
;*******************************************************************************
include kdtSegDef.inc
include devhlp.inc


;*******************************************************************************
;* Publics                                                                     *
;*******************************************************************************
public  kdtDHInit
public  kdtDHGetRouterFP
public  kdtStackThunk16To32
public  kdtStackThunk32To16
public  TKSSBase
public  pulTKSSBase32


;*******************************************************************************
;* External Globals                                                            *
;*******************************************************************************
extrn DOS32ALLOCMEM:PROC
extrn DOS32FREEMEM:PROC
extrn DOS16GETINFOSEG:PROC


;*******************************************************************************
;* Global Variables                                                            *
;*******************************************************************************
DATA32 segment
pulTKSSBase32 dd 0
DATA32 ends




CODE16 SEGMENT
    ASSUME cs:CODE16, ds:FLAT

;
;Jump table (near pointers)
;
apfnDHWorkers:
dw offset dh_stub               ; DevHlp_SchedClock       EQU      0      ;  0    Called each timer tick
dw offset dh_stub               ; DevHlp_DevDone          EQU      1      ;  1    Device I/O complete
dw offset dh_stub               ; DevHlp_Yield            EQU      2      ;  2    yield CPU if resched set
dw offset dh_stub               ; DevHlp_TCYield          EQU      3      ;  3    yield to time critical task
dw offset dh_stub               ; DevHlp_ProcBlock        EQU      4      ;  4    Block on event
dw offset dh_stub               ; DevHlp_ProcRun          EQU      5      ;  5    Unblock process
dw offset dh_stub               ; DevHlp_SemRequest       EQU      6      ;  6    claim a semaphore
dw offset dh_stub               ; DevHlp_SemClear         EQU      7      ;  7    release a semaphore
dw offset dh_stub               ; DevHlp_SemHandle        EQU      8      ;  8    obtain a semaphore handle
dw offset dh_stub               ; DevHlp_PushRequest      EQU      9      ;  9    Push the request
dw offset dh_stub               ; DevHlp_PullRequest      EQU     10      ;  A    Pull next request from Q
dw offset dh_stub               ; DevHlp_PullParticular   EQU     11      ;  B    Pull a specific request
dw offset dh_stub               ; DevHlp_SortRequest      EQU     12      ;  C    Push request in sorted order
dw offset dh_stub               ; DevHlp_AllocReqPacket   EQU     13      ;  D    allocate request packet
dw offset dh_stub               ; DevHlp_FreeReqPacket    EQU     14      ;  E    free request packet
dw offset dh_stub               ; DevHlp_QueueInit        EQU     15      ;  F    Init/Clear char queue
dw offset dh_stub               ; DevHlp_QueueFlush       EQU     16      ; 10    flush queue
dw offset dh_stub               ; DevHlp_QueueWrite       EQU     17      ; 11    Put a char in the queue
dw offset dh_stub               ; DevHlp_QueueRead        EQU     18      ; 12    Get a char from the queue
dw offset dh_stub               ; DevHlp_Lock             EQU     19      ; 13    Lock segment
dw offset dh_stub               ; DevHlp_Unlock           EQU     20      ; 14    Unlock segment
dw offset dh_stub               ; DevHlp_PhysToVirt       EQU     21      ; 15    convert physical address to virtual
dw offset dh_stub               ; DevHlp_VirtToPhys       EQU     22      ; 16    convert virtual address to physical
dw offset dh_stub               ; DevHlp_PhysToUVirt      EQU     23      ; 17    convert physical to LDT
dw offset dh_stub               ; DevHlp_AllocPhys        EQU     24      ; 18    allocate physical memory
dw offset dh_stub               ; DevHlp_FreePhys         EQU     25      ; 19    free physical memory
dw offset dh_stub               ; DevHlp_SetROMVector     EQU     26      ; 1A    set a ROM service routine vector
dw offset dh_stub               ; DevHlp_SetIRQ           EQU     27      ; 1B    set an IRQ interrupt
dw offset dh_stub               ; DevHlp_UnSetIRQ         EQU     28      ; 1C    unset an IRQ interrupt
dw offset dh_stub               ; DevHlp_SetTimer         EQU     29      ; 1D    set timer request handler
dw offset dh_stub               ; DevHlp_ResetTimer       EQU     30      ; 1E    unset timer request handler
dw offset dh_stub               ; DevHlp_MonitorCreate    EQU     31      ; 1F    create a monitor
dw offset dh_stub               ; DevHlp_Register         EQU     32      ; 20    install a monitor
dw offset dh_stub               ; DevHlp_DeRegister       EQU     33      ; 21    remove a monitor
dw offset dh_stub               ; DevHlp_MonWrite         EQU     34      ; 22    pass data records to monitor
dw offset dh_stub               ; DevHlp_MonFlush         EQU     35      ; 23    remove all data from stream
dw offset dh_GetDOSVar          ; DevHlp_GetDOSVar        EQU     36      ; 24    Return pointer to DOS variable
dw offset dh_stub               ; DevHlp_SendEvent        EQU     37      ; 25    an event occurred
dw offset dh_stub               ; DevHlp_ROMCritSection   EQU     38      ; 26    ROM Critical Section
dw offset dh_stub               ; DevHlp_VerifyAccess     EQU     39      ; 27    Verify access to memory
dw offset dh_stub               ; DevHlp_RAS              EQU     40      ; 28    Put info in RAS trace buffer
dw offset dh_stub               ; DevHlp_ABIOSGetParms    EQU     41      ; 29    Get ABIOS Calling Parms
dw offset dh_stub               ; DevHlp_AttachDD         EQU     42      ; 2A    Attach to a device driver
dw offset dh_stub               ; DevHlp_InternalError    EQU     43      ; 2B    Signal an internal error
dw offset dh_stub               ; DevHlp_ModifyPriority   EQU     44      ; 2C    Undocumented (used by PM)
dw offset dh_stub               ; DevHlp_AllocGDTSelector EQU     45      ; 2D    Allocate GDT Selectors
dw offset dh_stub               ; DevHlp_PhysToGDTSelector EQU    46      ; 2E    Convert phys addr to GDT sel
dw offset dh_stub               ; DevHlp_RealToProt       EQU     47      ; 2F    Change from real to protected mode
dw offset dh_stub               ; DevHlp_ProtToReal       EQU     48      ; 30    Change from protected to real mode
dw offset dh_stub               ; DevHlp_EOI              EQU     49      ; 31    Send EOI to PIC
dw offset dh_stub               ; DevHlp_UnPhysToVirt     EQU     50      ; 32    mark completion of PhysToVirt
dw offset dh_stub               ; DevHlp_TickCount        EQU     51      ; 33    modify timer
dw offset dh_stub               ; DevHlp_GetLIDEntry      EQU     52      ; 34    Obtain Logical ID
dw offset dh_stub               ; DevHlp_FreeLIDEntry     EQU     53      ; 35    Release Logical ID
dw offset dh_stub               ; DevHlp_ABIOSCall        EQU     54      ; 36    Call ABIOS
dw offset dh_stub               ; DevHlp_ABIOSCommonEntry EQU     55      ; 37    Invoke Common Entry Point
dw offset dh_stub               ; DevHlp_GetDeviceBlock   EQU     56      ; 38    Get ABIOS Device Block
dw offset dh_stub               ;                                         ; 39    Reserved for Profiling Kernel
dw offset dh_stub               ; DevHlp_RegisterStackUsage EQU   58      ; 3A    Register for stack usage
dw offset dh_stub               ; DevHlp_LogEntry         EQU     59      ; 3B    Place data in log buffer
dw offset dh_stub               ; DevHlp_VideoPause       EQU     60      ; 3C Video pause on/off      - D607
dw offset dh_stub               ; DevHlp_Save_Message     EQU     61      ; 3D    Save msg in SysInit Message Table
dw offset dh_stub               ; DevHlp_SegRealloc       EQU     62      ; 3E    Realloc DD protect mode segment
dw offset dh_stub               ; DevHlp_PutWaitingQueue  EQU     63      ; 3F    Put I/O request on waiting queue
dw offset dh_stub               ; DevHlp_GetWaitingQueue  EQU     64      ; 40    Get I/O request from waiting queue
dw offset dh_stub               ; DevHlp_PhysToSys        EQU     65      ; 41    Address conversion for the AOX
dw offset dh_stub               ; DevHlp_PhysToSysHook    EQU     66      ; 42    Address conversion for the AOX
dw offset dh_stub               ; DevHlp_RegisterDeviceClass EQU     67      ; 43    Register DC entry point
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     68
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     69
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     70
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     71
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     72
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     73
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     74
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     75
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     76
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     77
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     78
dw offset dh_stub               ; Reserved for 16-bit dhs EQU     79
dw offset dh_stub               ; DevHlp_RegisterPDD      EQU     80      ; 50    Register PDD entry point with VDM manager for later PDD-VDD communication
dw offset dh_stub               ; DevHlp_RegisterBeep     EQU     81      ; 51    register PTD beep service entry point with kernel
dw offset dh_stub               ; DevHlp_Beep             EQU     82      ; 52    preempt beep service via PTD
dw offset dh_stub               ; DevHlp_FreeGDTSelector  EQU     83      ; 53    Free allocated GDT selector
dw offset dh_stub               ; DevHlp_PhysToGDTSel     EQU     84      ; 54    Convert Phys Addr to GDT sel with given access
dw offset dh_VMLock             ; DevHlp_VMLock           EQU     85      ; 55    Lock linear address range
dw offset dh_VMUnlock           ; DevHlp_VMUnlock         EQU     86      ; 56    Unlock address range
dw offset dh_VMAlloc            ; DevHlp_VMAlloc          EQU     87      ; 56    Allocate memory
dw offset dh_VMFree             ; DevHlp_VMFree           EQU     88      ; 58    Free memory or mapping
dw offset dh_stub               ; DevHlp_VMProcessToGlobal EQU    89      ; 59    Create global mapping to process memory
dw offset dh_stub               ; DevHlp_VMGlobalToProcess EQU    90      ; 5A    Create process mapping to global memory
dw offset dh_VirtToLin          ; DevHlp_VirtToLin        EQU     91      ; 5B Convert virtual address to linear
dw offset dh_stub               ; DevHlp_LinToGDTSelector EQU     92      ; 5C Convert linear address to virtual
dw offset dh_stub               ; DevHlp_GetDescInfo      EQU     93      ; 5D Return descriptor information
dw offset dh_stub               ; DevHlp_LinToPageList    EQU     94      ; 5E build pagelist array from lin addr
dw offset dh_stub               ; DevHlp_PageListToLin    EQU     95      ; 5F map page list array to lin addr
dw offset dh_stub               ; DevHlp_PageListToGDTSelector EQU    96  ; 60 map page list array to GDT sel.
dw offset dh_stub               ; DevHlp_RegisterTmrDD    EQU     97      ; 61 Register TMR Device Driver.
dw offset dh_stub               ; DevHlp_RegisterPerfCtrs EQU     98      ; 62 Register device driver perf. ctrs (PVW).
dw offset dh_stub               ; DevHlp_AllocateCtxHook  EQU     99      ; 63 Allocate a context hook
dw offset dh_stub               ; DevHlp_FreeCtxHook      EQU     100     ; 64 Free a context hook
dw offset dh_stub               ; DevHlp_ArmCtxHook       EQU     101     ; 65 Arm a context hook
dw offset dh_stub               ; DevHlp_VMSetMem         EQU     102     ; 66H commit/decommit memory
dw offset dh_stub               ; DevHlp_OpenEventSem     EQU     103     ; 67H open an event semaphore
dw offset dh_stub               ; DevHlp_CloseEventSem    EQU     104     ; 68H close an event semaphore
dw offset dh_stub               ; DevHlp_PostEventSem     EQU     105     ; 69H post an event semaphore
dw offset dh_stub               ; DevHlp_ResetEventSem    EQU     106     ; 6AH reset an event semaphore
dw offset dh_stub               ; DevHlp_RegisterFreq     EQU     107     ; 6BH   register PTD freq service entry point with kernel
dw offset dh_stub               ; DevHlp_DynamicAPI       EQU     108     ; 6CH add a dynamic API
dw offset dh_stub               ; DevHlp_ProcRun2         EQU     109     ; 6DH  Unblock process via procrun2
dw offset dh_stub               ; DevHlp_CreateInt13VDM   EQU     110 ;6EH Create Int13 VDM (Internal Only) OEMINT13
dw offset dh_stub               ; DevHlp_RegisterKrnlExit EQU     111 ;6FH Used to capture Kernel Exits   F78693
dw offset dh_stub               ; DevHlp_PMPostEventSem   EQU     112     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     113     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     114     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     115     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     116     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     117     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     118     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     119     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     120     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     121     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     122     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     123     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; reserved other os2 vers EQU     124     ; 70h  PM Post Event Semaphore
dw offset dh_stub               ; DevHlp_KillProc         EQU     125 ;7DH Kill Proc                        ;@SEC
dw offset dh_stub               ; DevHlp_QSysState        EQU     126 ;7EH Query System State               ;@SEC
dw offset dh_stub               ; DevHlp_OpenFile         EQU     127     ; 7FH  Ring-0 File system Write
dw offset dh_stub               ; DevHlp_CloseFile        EQU     128     ; 80H  Ring-0 File system Seek
dw offset dh_stub               ; DevHlp_ReadFile         EQU     129     ; 81H  Ring-0 File system Read
dw offset dh_stub               ; DevHlp_ReadFileAt       EQU     130     ; 82H  File system Read at (seek)
apfnDHWorkersEnd db 0



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
if 0
    cmp     dl, 130
    jle     dhr_ok
    mov     eax, 87
    stc

dhr_ok:
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
    push    ebp

    push    dx
    mov     ebp, esp
    and     word ptr ss:[ebp], 00ffh
    shl     word ptr ss:[ebp], 1
    add     word ptr ss:[ebp], offset CODE16:apfnDHWorkers
    call    word ptr ss:[ebp]
    add     esp, 2

    ;
    ; Restore nearly everything (as a start)
    ;
    pop     ebp
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
else
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
endif
DHRouter ENDP


dh_printf proc near

dh_printf endp



;;
; Stub function which gets called when we encounter an unimplemented
; devhelper.
;
; @returns  eax=87 cf
;
dh_stub proc near
;    DPRINTF<"dh_stub: %c"
    ; todo - write debug info.
    int 3
    mov     eax, 87
    stc
    ret
dh_stub endp




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
db "DosTables:"
;
; The two dostables.
;
public DosTable
DosTable                        db 17
  DosTable_fph_HardError        dd 0
public DosTable_fph_HardError       
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

public DosTable2
DosTable2                       db  19
public DosTable2_fpErrMap24 
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
public DosTable2_R0FlatCS   
                                dw 0
  DosTable2_R0FlatDS            dw seg FLAT:DATA32
                                dw 0
  DosTable2_pTKSSBase           dd offset FLAT:DATA32:TKSSBase
public DosTable2_pintSwitchStack
  DosTable2_pintSwitchStack     dd offset FLAT:CODE32:intSwitchStack
  DosTable2_pprivatStack        dd offset FLAT:DATA32:pPrivateStack
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
; @cproto    void _Optlink kdtDHInit(void)
; @returns   NO_ERROR.
; @param     none.
; @uses      eax
; @sketch
; @status    completly implemented
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
kdtDHInit PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING

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
kdtDHInit ENDP


;;
; Gets the a 16-bit far pointer to the DHRouter routine.
; @cproto    ULONG  _Optlink kdtDHGetRouterFP(VOID); /* dh.asm */
; @returns   16-bit far pointer to the DHRouter routine.
; @uses      EAX
; @status    completely implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
kdtDHGetRouterFP PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING
    mov     ax, seg CODE16:DHRouter
    shl     eax, 16
    mov     ax, offset CODE16:DHRouter
    ret
kdtDHGetRouterFP ENDP





;;
; Thunks the stack from 16-bits to 32-bits.
; !The stack has to be 16-bits on entry!
; @cproto    VOID _Optlink kdtStackThunk16To32(VOID)
; @returns   void
;            Stack is 32-bits!
;            EBP and SS:ESP is thunked
;
; @uses      EAX, EDX
; @sketch
; @status    completly implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    RING 3 only.
kdtStackThunk16To32 PROC NEAR
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
kdtStackThunk16To32 ENDP


;;
; Thunks the stack from 32-bits to 16-bits.
; !The stack has to be 32-bits on entry!
; @cproto    VOID _Optlink kdtStackThunk32To16(VOID)
; @returns   void
;            Stack is 16-bits!
;            EBP and SS:ESP is thunked
;
; @uses      EAX, EDX
; @sketch
; @status    completly implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    RING 3 only.
kdtStackThunk32To16 PROC NEAR
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
kdtStackThunk32To16 ENDP

DATA32 segment
  db "intSwitchStack-Data:"
OS2Stack_ESP dd 0                       ; OS/2 kernel stack 48 ptr
OS2Stack_SS dw 0
pPrivateStackNew_ESP dd 0               ; private stack 48 ptr
pPrivateStackNew_SS dw 0
pPrivateStack dd 0                      ; private stack ESP (flat)
DATA32 ends

public intSwitchStack
intSwitchStack proc near
    or      eax, eax
    jz      switch_back
    
switch_to:
    mov     dx, ds
    cmp     dx, seg FLAT:DATA32
    jz      dsok
    int 3
dsok:
    mov     pPrivateStackNew_SS, ds
    mov     pPrivateStackNew_ESP, eax
    pop     eax
    push    OS2Stack_SS
    push    OS2Stack_ESP
    mov     OS2Stack_SS, ss
    mov     OS2Stack_ESP, esp
    lss     esp, fword ptr pPrivateStackNew_ESP
    jmp     eax
    
switch_back:
    pop     eax
    mov     pPrivateStack, esp
    lss     esp, fword ptr OS2Stack_ESP
    pop     word ptr OS2Stack_SS
    pop     dword ptr OS2Stack_ESP
    jmp     eax
intSwitchStack endp

CODE32 ENDS

end


