; $Id: myVR32AllocMem.asm,v 1.2 2001-01-20 15:48:54 bird Exp $
;
; VR32AllocMem over loader which adds the OBJ_ANY flag.
;
; Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p


;
;   Defined Constants And Macros
;
SEF_EIP     EQU     3ch
SEF_CS      EQU     40h

ARG_FLAGS   EQU     50h


;
; Include files
;
    include devsegdf.inc
    include bsememf.inc
    ifndef OBJ_ANY
    OBJ_ANY	EQU	00000400H
    endif


;
; Exported symbols
;
    public _myVR32AllocMem@50


;
; Externs
;
    extrn apiApplyChange:PROC           ; system call?
    extrn _VR32AllocMem@50:PROC


CODE32 segment


_myVR32AllocMem@50 proc near
    ASSUME ds:FLAT, es:NOTHING, ss:NOTHING
    ;
    ; Check if OBJ_ANY flag is allready set.
    ;
    mov     edx, [esp + ARG_FLAGS]
    test    edx, OBJ_ANY
    jnz     FLAT:call_vr


    ;
    ; It was not - check if we're to set it for this calling module.
    ;
    push    eax
    push    dword ptr [esp + SEF_CS]
    push    dword ptr [esp + SEF_EIP]
    push    1
    call    apiApplyChange
    sub     esp, 0ch
    test    eax, eax
    pop     eax
    jz      FLAT:call_vr

    ; Apply the OBJ_ANY flag.
    or      word ptr [esp + ARG_FLAGS], OBJ_ANY ; only need to update first word.

    ;
    ; Call the original verify/worker routine.
    ;
call_vr:
    jmp     FLAT:CALLTAB:_VR32AllocMem@50
_myVR32AllocMem@50 endp

CODE32 ends

end
