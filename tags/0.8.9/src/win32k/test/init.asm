; $Id: init.asm,v 1.1 2000-07-16 22:18:15 bird Exp $
;
; Thunks to call stratgy entry points with init packets.
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
    include os2.inc


;
; Exported symbols
;
    public  InitElf
    public  InitWin32k


;
; extrns
;
    extrn   _strategyAsm0:PROC
    extrn   _strategyAsm1:PROC

CODE32 SEGMENT
;;
; Initiates the $elf device driver.
; @cproto    ULONG _Optlink InitElf(PVOID pvPacket);
; @returns
; @param     (ss:)eax    pvPacket
;                   This parameter is pointer to a stack object which haven't
;                   had SSToDS() applied. This is done in this way because it
;                   lightens the job to make a far 16 bits pointer of it.
; @uses      eax, ecx, edx
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
InitElf PROC NEAR
    ASSUME ds:FLAT, es:NOTHING, ss:NOTHING
    push    ebx
    push    ds
    push    es
    ;
    ; Make 16:16 ptr to packet pointed to by ss:eax
    ;
    mov     bx, ss
    mov     es, bx
    mov     bx, ax

    ;
    ; Set ds to DATA16
    ;
    mov     ax, seg DATA16
    mov     ds, ax

    ;
    ; Call stratgy entry point.
    ;
    jmp far ptr CODE16:Thunk16_InitElf
Thunk32_InitElf::
    movzx   eax, ax                     ; Cast USHORT to ULONG.

    pop     es
    pop     ds
    pop     ebx
    ret
InitElf ENDP
CODE32 ENDS

CODE16 SEGMENT
    ; Call strategy entry point for $elf.
Thunk16_InitElf::
    call    far ptr CODE16:_strategyAsm0
    jmp far ptr FLAT:Thunk32_InitElf
CODE16 ENDS



CODE32 SEGMENT
;;
; Initiates the $win32k device driver.
; @cproto    ULONG _Optlink InitWin32k(PVOID pvPacket);
; @returns
; @param     (ss:)eax    pvPacket
;                   This parameter is pointer to a stack object which haven't
;                   had SSToDS() applied. This is done in this way because it
;                   lightens the job to make a far 16 bits pointer of it.
; @uses      eax, ecx, edx
; @sketch
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
InitWin32k PROC NEAR
    ASSUME ds:FLAT, es:NOTHING, ss:NOTHING
    push    ebx
    push    ds
    push    es
    ;
    ; Make 16:16 ptr to packet pointed to by ss:eax
    ;
    mov     bx, ss
    mov     es, bx
    mov     bx, ax

    ;
    ; Set ds to DATA16
    ;
    mov     ax, seg DATA16
    mov     ds, ax

    ;
    ; Call stratgy entry point.
    ;
    jmp far ptr CODE16:Thunk16_InitWin32k
Thunk32_InitWin32k::
    movzx   eax, ax                     ; Cast USHORT to ULONG.

    pop     es
    pop     ds
    pop     ebx
    ret
InitWin32k ENDP
CODE32 ENDS

CODE16 SEGMENT
    ; Call strategy entry point for $win32k.
Thunk16_InitWin32k::
    call    far ptr CODE16:_strategyAsm1
    jmp far ptr FLAT:Thunk32_InitWin32k

CODE16 ENDS


END
