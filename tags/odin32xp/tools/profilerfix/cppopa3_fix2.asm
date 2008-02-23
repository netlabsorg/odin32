; $Id: cppopa3_fix2.asm,v 1.3 2002-04-12 00:18:33 bird Exp $
;
; Fix for the DosLoadModule traps in debugee
; during tracing init.
;
;
; NOTE!!! Currently the address of the doscall1 init proc is
;         HARDCODED.
;
;
; Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
;
; Project Odin Software License can be found in LICENSE.TXT
;

        .386
		.model flat

;
;   Defined Constants And Macros
;
        large           equ

        SEL_FLATMASK    equ     01fff0000h
        SEL_FLAT_SHIFT  equ     0dh
        SEL_LDT_RPL3    equ     07h

        DOSCALL1_INIT_ADDR      equ 01c0209d0h
        DOSCALL1_pTLMA0_ADDR    equ 0130111a0h

;
; Public symbols
;
        public prfLoadModule


;
; Externs
;
        extrn Dos32LoadModule:PROC
        extrn DOS16LOADMODULE:far


;
;  declare 16-bit data segment..
;
CODE16 segment word public 'CODE' use16
aDoscall1       db 'DOSCALL1',0
CODE16 ends


;
; 32-bit data segment
;
DATA32 segment dword public use32
;    fDosCallnited  dd 0
DATA32 ends


;
; 32-bit code segement.
;
CODE32 segment dword public 'CODE' use32		
        assume cs:CODE32
		assume es:nothing, ss:nothing, ds:nothing, fs:nothing, gs:nothing


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
; @uses      EAX
; @sketch
; @status    completly implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    RING 3 only.
ThunkStack32To16 PROC NEAR
    ASSUME cs:CODE32, ds:FLAT, es:NOTHING, ss:NOTHING

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

    ret
ThunkStack32To16 ENDP


;;
;   APIRET APIENTRY  DosLoadModule(PCSZ         pszName,                ebp +008h
;                                  ULONG        cbName,                 ebp +00ch
;                                  PCSZ         pszModname,             ebp +010h
;                                  PHMODULE     phmod);                 epb +014h
prfLoadModule proc near
    ASSUME ss:NOTHING, ds:FLAT, cs:CODE32

    ;
    ; Only first time!
    ;
;    test    fDosCallnited, 0ffh
    mov     eax, DOSCALL1_pTLMA0_ADDR
    cmp     dword ptr [eax], 0
    jne     Dos32LoadModule
;    mov     fDosCallnited, 0ffh

    ;
    ; Save registers
    ;
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp

    ;
    ; Make stack 16-bit.
    ;
    call    ThunkStack32To16


    ;
    ; Load DosCall1.DLL. ie. get handle and setup callstack for init.
    ;
    jmp far ptr CODE16:prfLoadDosCall1DLL_Thunk16
CODE32 ends
CODE16 segment word public 'CODE' use16
prfLoadDosCall1DLL_Thunk16::
    ASSUME ss:NOTHING, ds:FLAT, cs:CODE16
    ;
    ; Load DosCall1.
    ;
    push    0                           ; init flag
    push    0

    push    0                           ; handle
    push    0
    mov     ax, sp

    push    0                           ; fail seg
    push    0                           ; fail off
    push    0                           ; fail len

    push    cs
    push    offset CODE16:aDoscall1     ; module name.

    push    ss
    push    ax                          ; handle
    call far ptr DOS16LOADMODULE
CODE16 ends
CODE32 segment
prfLoadDosCall1DLL_Thunk32::
    mov     eax, DOSCALL1_INIT_ADDR     ; hardcoded for now.
    call dword ptr eax
    add     esp, 08h


    ;
    ; Make stack 32-bit.
    ;
    call    ThunkStack16To32

    ;
    ; Restore registers.
    ;
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    jmp     Dos32LoadModule
prfLoadModule endp

CODE32 ends

;
; 16-bit code segement.
;
CODE16 segment
        jmp far ptr FLAT:prfLoadDosCall1DLL_Thunk32
CODE16 ends

		end
