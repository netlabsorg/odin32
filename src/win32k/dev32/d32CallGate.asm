; $Id: d32CallGate.asm,v 1.5.2.1 2001-09-27 03:08:12 bird Exp $
;
; 32-bit CallGate used to communitcate fast between Ring-3 and Ring-0.
; This module contains all assembly workers for this.
;
; Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;

    .386p

;
;   Defined Constants And Macros
;
    INCL_ERRORS EQU 1


;
;   Header Files
;
    include bseerr.inc
    include devsegdf.inc
    include devhlp.inc
    include win32k.inc


;
; Exported symbols
;
    public CallGateGDT

    public InitCallGate
    public Win32kAPIRouter


;
; External symbols
;
    extrn  _Device_Help:dword
    extrn  _TKSSBase:dword

    extrn  KMEnterKmodeSEF:near
    extrn  KMExitKmodeSEF8:near
    extrn  _TKFuBuff@16:near

    extrn  k32AllocMemEx:near
    extrn  k32QueryOTEs:near
    extrn  k32QueryOptionsStatus:near
    extrn  k32SetOptions:near
    extrn  k32ProcessReadWrite:near
    extrn  k32HandleSystemEvent:near
    extrn  k32QuerySystemMemInfo:near
    extrn  k32QueryCallGate:near
    extrn  k32SetEnvironment:near
    extrn  k32KillProcessEx:near


;
;   Global Variables
;
DATA16 segment
CallGateGDT     dw  0                   ; GDT used for the 32-bit Ring-3 -> Ring-0 call gate.
DATA16 ends

DATA32 segment
GDTR_limit      dw ?                    ; The limit field of the GDTR.
GDTR_base       dd ?                    ; The base field of the GDTR. (linear flat address)


;
; Structure containing the K32 API parameter packet size.
;
; Used for parameter packet validation, and for copying the parameter
; packet from user address space into system address space (the stack).
;
acbK32Params:
    dd 0                                ; Not used - ie. invalid
    dd SIZE   K32ALLOCMEMEX             ; K32_ALLOCMEMEX          0x01
    dd SIZE   K32QUERYOTES              ; K32_QUERYOTES           0x02
    dd SIZE   K32QUERYOPTIONSSTATUS     ; K32_QUERYOPTIONSSTATUS  0x03
    dd SIZE   K32SETOPTIONS             ; K32_SETOPTIONS          0x04
    dd SIZE   K32PROCESSREADWRITE       ; K32_PROCESSREADWRITE    0x05
    dd SIZE   K32HANDLESYSTEMEVENT      ; K32_HANDLESYSTEMEVENT   0x06
    dd SIZE   K32QUERYSYSTEMMEMINFO     ; K32_QUERYSYSTEMMEMINFO  0x07
    dd SIZE   K32QUERYCALLGATE          ; K32_QUERYCALLGATE       0x08
    dd SIZE   K32SETENVIRONMENT         ; K32_SETENVIRONMENT      0x09
    dd SIZE   K32KILLPROCESSEX          ; K32_KILLPROCESSEX       0x0a

;
; Structure containing the offsets of K32 API worker routines.
;
; Used for calling the workers indirectly.
;
apfnK32APIs:
    dd  FLAT:k32APIStub                 ; Not used - ie. invalid
    dd  FLAT:k32AllocMemEx              ; K32_ALLOCMEMEX          0x01
    dd  FLAT:k32QueryOTEs               ; K32_QUERYOTES           0x02
    dd  FLAT:k32QueryOptionsStatus      ; K32_QUERYOPTIONSSTATUS  0x03
    dd  FLAT:k32SetOptions              ; K32_SETOPTIONS          0x04
    dd  FLAT:k32ProcessReadWrite        ; K32_PROCESSREADWRITE    0x05
    dd  FLAT:k32HandleSystemEvent       ; K32_HANDLESYSTEMEVENT   0x06
    dd  FLAT:k32QuerySystemMemInfo      ; K32_QUERYSYSTEMMEMINFO  0x07
    dd  FLAT:k32QueryCallGate           ; K32_QUERYCALLGATE       0x08
    dd  FLAT:k32SetEnvironment          ; K32_SETENVIRONMENT      0x09
    dd  FLAT:k32KillProcessEx           ; K32_KILLPROCESSEX       0x0a
DATA32 ends



CODE32 segment
    assume cs:CODE32, ds:FLAT ;, es:nothing, ss:nothing

;;
; This function initiates the callgate.
; @cproto   extern ULONG _Optlink InitCallGate(void);
; @returns  0 (NO_ERROR) on success.
;           appropriate error code on error.
; @uses     eax, ecx, edx
; @sketch
; @status
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark
InitCallGate proc near
    push    ebp
    mov     ebp, esp
    push    edi
    push    esi
    push    ebx
    push    ds
    push    es


    ;
    ; Allocate GDT selector for the call gate.
    ; (URG! This call also allocates 68kb of virtual memory which i don't need!)
    ;
    mov     di, seg DATA16:CallGateGDT
    mov     es, di
    mov     di, offset DATA16:CallGateGDT
    mov     cx, 1
    mov     dl, DevHlp_AllocGDTSelector
    jmp     far ptr CODE16:Thunk16_AllocGDTSelector
Thunk32_AllocGDTSelector::
    jnc     ICG_allocok
    movzx   eax, ax                     ; We failed, zero high word of eax to return proper return code.
    jmp     ICG_end                     ; bail out.

    ;
    ; We successfully allocate the callgate GDT.
    ; How we'll find the descriptor entry for it.
    ;
ICG_allocok:
    ASSUME  ds:FLAT
    sgdt    GDTR_limit                  ; Get the GDTR content.
    mov     ax, GDTR_limit
    mov     ebx, GDTR_base
    movzx   ecx, CallGateGDT
    and     cx, 0fff8h                  ; clear the dpl bits and descriptor type bit. (paranoia!)
    cmp     cx, ax                      ; check limit. (paranoia!!!)
    jl      ICG_limitok
    mov     eax, 0ffffffffh             ; return failure.
    jmp     ICG_end
ICG_limitok:
    add     ebx, ecx                    ; GDTR_base + selector offset -> flat pointer to selector.

    ;
    ; ebx is pointing to the descriptor table entry for my GDT selector.
    ; Now we'll have to change it into a callgate.
    ; This is the layout of a callgate descriptor:
    ;  bits
    ;  0-15  Segment offset low word
    ; 16-31  Segment selector
    ; -------second dword-------
    ;   0-4  Param Count
    ;   5-7  Reserved (zero)
    ;  8-11  Selector type - 1100b
    ;    12  Reserved (UVirt) zero
    ; 13-14  Descriptor Privelege Level (DPL)
    ;    15  Present flag / Gate valid.
    ; 16-31  Segment offset high word.
    ;
    mov     eax, offset FLAT:Win32kCallGate
    mov     word ptr [ebx], ax          ; set low segment offset word
    shr     eax, 10h
    mov     word ptr [ebx + 6], ax      ; set high segment offset word

    mov     word ptr [ebx + 4], 1110110000000010b ; set flags and stuff.
                                        ; param count:  Two (2) - parameter struct and function number
                                        ; type:         32-bit call gate
                                        ; DPL:          Ring-3
                                        ; Gate Valid:   Yes
    mov     word ptr [ebx + 2], seg FLAT:CODE32 ; Set the selector to FLAT Ring-0 code selector.
    xor     eax, eax                    ; return successfully.

ICG_end:
    pop     es
    pop     ds
    pop     ebx
    pop     esi
    pop     edi
    leave
    ret
InitCallGate endp



;;
; This is the callgate procedure.
; @cproto   none
; @returns  return value of the callgate router.
; @param    fill inn later....
; @uses     eax, ecx, edx
; @sketch
; @status
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
; @remark
;   stack frame - before KMEnterKmodeSEF:
;       --bottom of stack---
;       calling ss                                                1ch
;       calling esp                                               18h
;       pParameter          (parameter 1)                         14h
;       ulFunctionCode      (parameter 0)                         10h
;       calling cs                                                0ch
;       calling eip         <-- esp points here upon entry.        8h
;       ---top of stack---
;       flags               (pushf)                                4h
;       parameter size      (push 8h)                              0h
;
;  After the call to KMEnterKmodeSEF:
;       --bottom of stack---
;       calling ss                                                50
;       calling esp                                               4c
;       pParameter          (parameter 1)                         48
;       ulFunctionCode      (parameter 0)                         44
;       sef_cs                                                    40
;       sef_eip                                                   3c
;       sef_eflag                                                 38
;       sef_cbargs                                                34
;       sef_retaddr                                               30
;       sef_ds                                                    2c
;       sef_es                                                    28
;       sef_fs                                                    24
;       sef_gs                                                    20
;       sef_eax                                                   1c
;       sef_ecx                                                   18
;       sef_edx                                                   14
;       sef_ebx                                                   10
;       sef_padesp                                                 c
;       sef_ebp                                                    8
;       sef_esi                                                    4h
;       sef_edi                                                    0h
;
Win32kCallGate proc near
    ASSUME  ds:nothing, ss:nothing
    pushfd                              ; Push all flags (eflags)
    push    8h                          ; Size of parameters.

    call    KMEnterKmodeSEF             ; This is an OS2 kernel function which does
                                        ; kernel entry housekeeping.

    mov     edx, [esp + 48h]            ; pParameter (parameter 1)
    mov     eax, [esp + 44h]            ; ulFunctionCode (parameter 2)
    sub     esp, 8h                     ; (Even when using _Oplink we have to reserve space for parameters.)
    call    Win32kAPIRouter             ; This is my Ring-0 api router.
    add     esp, 8h

    jmp     KMExitKmodeSEF8             ; This a an OS2 kernel function which does
                                        ; kernel exist housekeeping.
Win32kCallGate endp


;;
; Internal function router which calls the correct function.
; Called from IOCtl worker in d32Win32kIOCtl.c and callgate.
; @cproto   APIRET _Optlink Win32kAPIRouter(ULONG ulFunction, PVOID pvParam);
; @returns  function return code.
;           0xdeadbeef if invalid function number.
; @param    eax - ulFunction    Function number to call.
; @param    edx - pvParam       Parameter packet for that function.
; @uses     eax, edx, ecx
; @sketch   Validate function number
;           Fetch the parameter pacted from user mode and place it on the stack.
;           Validate the size field of the parameter packet.
;           Remove the packet header from the stack => we have a callframe for the api.
;           Call the API worker.
;           Return.
; @status   Completely implemented.
; @author   knut st. osmundsen (knut.stange.osmundsen@mynd.no)
Win32kAPIRouter proc near
    ASSUME  ds:FLAT, es:nothing, ss:nothing
    ;
    ; Validate function number.
    ;
    test    eax,eax
    jnz     APIR_notnull                ; This code should be faster (though it may look stupid to
                                        ; jump around like this). IIRC branch prediction allways
                                        ; takes a branch. And btw there are 4 NOPs after this jump!
    jmp     APIR_InvalidFunction

APIR_notnull:
    cmp     eax, K32_LASTIOCTLFUNCTION
    jle     APIR_ValidFunction
APIR_InvalidFunction:
    mov     eax, 0deadbeefh
    ret

    ;
    ; We have a valid function number now.
    ; Copy the parameter struct on to the stack.
    ;
APIR_ValidFunction:
    push    ebp                         ; Make stack frame
    mov     ebp, esp
    mov     [ebp+8], eax                ; Save eax on the stack (reserved by caller according to _Optlink)
    mov     ecx, acbK32Params[eax*4]    ; ecx <- size of parameter packet.
    sub     esp, ecx                    ; Reserve stack space for the parameter packet.
    mov     eax, _TKSSBase              ; Calculate the FLAT address of esp.
    add     eax, esp                    ; eax <- flat address of ss:esp
    push    ecx                         ; Save the size.
    ; TKFuBuff(pv, pvParam, acbParams[ulFunction], TK_FUSU_NONFATAL);
    push    0                           ; TK_FUSU_NOFATAL
    push    ecx                         ; Size of parameter packet
    push    edx                         ; Pointer to user memory to fetch
    push    eax                         ; Pointer to target memory.
    call    _TKFuBuff@16                ; __stdcall (cleanup done by the called function)
    pop     ecx                         ; Restore size
    test    eax, eax
    jz      APIR_FetchOK
    jmp     APIR_end

    ;
    ; Parameter packet is now read onto the stack. esp is pointing to it.
    ; Check the size of the struct as the caller sees it.
    ;
APIR_FetchOK:
    cmp     ecx, [esp]                  ; (esp now point at the parameter struct)
    je      APIR_sizeok
    mov     eax, ERROR_BAD_ARGUMENTS    ; return code.
    jmp     APIR_end

    ;
    ; The size is correct.
    ; Call the worker and return.
    ;
APIR_sizeok:
    add     esp, SIZE K32HDR            ; Skip the parameter header.
    mov     eax, [ebp + 8]              ; Restore function number.
    mov     eax, apfnK32APIs[eax*4]     ; eax <- address of the K32 API worker.
    call    eax                         ; Call the worker.
                                        ; No cleanup needed as leave takes care of that
                                        ; We're ready for returning.
APIR_end:
    leave
    ret
Win32kAPIRouter endp


;;
; This is a stub function which does nothing but returning an error code.
; @return       ERROR_NOT_SUPPORTED
k32APIStub proc near
    mov     eax, ERROR_NOT_SUPPORTED
    ret
k32APIStub endp

CODE32 ends





CODE16 segment
    assume cs:CODE16, ds:FLAT

;
; Thunker used by the InitCallGate procedure call the AllocGDTSelector devhelper.
;
Thunk16_AllocGDTSelector:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_AllocGDTSelector

CODE16 ends

end

