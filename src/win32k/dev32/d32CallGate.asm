; $Id: d32CallGate.asm,v 1.1 2001-02-20 05:00:13 bird Exp $
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
;   Header Files
;
    include devsegdf.inc
    include devhlp.inc


;
; Exported symbols
;
    public InitCallGate


;
; extrns
;
    extrn _Device_Help:dword
    extrn  KMEnterKmodeSEF:near
    extrn  KMExitKmodeSEF8:near
    extrn  Win32kAPIRouter:near


;
;   Global Variables
;
DATA16 segment
CallGateGDT     dw  0                   ; GDT used for the 32-bit Ring-3 -> Ring-0 call gate.
DATA16 ends

DATA32 segment
GDTR_limit      dw ?                    ; The limit field of the GDTR.
GDTR_base       dd ?                    ; The base field of the GDTR. (linear flat address)
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
    sub     esp, 10h
    push    edi
    push    esi
    push    ebx
    push    ds
    push    es

    ;
    ; Allocate GDT selector for the call gate.
    ; (seems like this call also allocates 68kb of virtual memory which i don't need.)
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
    pop     ds
    push    ds                          ; restore ds (make sure it's flat)
    ASSUME  ds:FLAT
    sgdt    GDTR_limit                  ; Get the GDTR content.
    mov     ax, GDTR_limit
    mov     ebx, GDTR_base
    movzx   ecx, CallGateGDT
    and     cx, 0fff8h                  ; clear the dpl bits and descriptor type bit.
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
    pop     ds
    pop     es
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
;   stack frame:
;       ---top of stack---
;       calling ss                                                1ch
;       calling esp                                               18h
;       pParameter          (parameter 1)                         14h
;       ulFunctionCode      (parameter 0)                         10h
;       calling cs                                                0ch
;       calling eip         <-- esp points here upon entry.        8h
;       ---top of stack---
;       flags               (pushf)                                4h
;       parameter size      (push 8h)                              0h
;       ---I start repushing parameters here.
;
;
Win32kCallGate proc near
    pushf                               ; Push all flags
    push    8h                          ; Size of parameters.

    call    KMEnterKmodeSEF             ; This is an OS2 kernel function which does
                                        ; kernel entry housekeeping.

    mov     edx, [esp + 14h]            ; pParameter (parameter 1)
    mov     eax, [esp + 10h]            ; ulFunctionCode (parameter 2)
    sub     esp, 8h                     ; (Even when using _Oplink we have to reserve space for parameters.)
    call    Win32kAPIRouter             ; This is my Ring-0 api. (d32Win32kIOCtl.c)
    add     esp, 8h

    jmp     KMExitKmodeSEF8             ; This a an OS2 kernel function which does
                                        ; kernel exist housekeeping.
Win32kCallGate endp


CODE32 ends





CODE16 segment
    assume cs:CODE16, ds:nothing, ss:nothing, es:nothing

;
; Thunker used by the InitCallGate procedure call the AllocGDTSelector devhelper.
;
Thunk16_AllocGDTSelector:
    call [_Device_Help]
    jmp far ptr FLAT:Thunk32_AllocGDTSelector

CODE16 ends

end

