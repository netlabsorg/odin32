; $Id: Init.asm,v 1.1 2001-09-17 01:11:05 bird Exp $
;
; Thunks to call stratgy entry points with init packets.
;
; Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)
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
    public  InitkKrnlHlp
    public  InitkKrnlLib


;
; extrns
;
    extrn   _strategyAsm0:PROC
    extrn   _strategyAsm1:PROC

CODE32 SEGMENT
;;
; Initiates the kKrnlHlp device driver.
; @cproto    ULONG _Optlink InitkKrnlHlp(PVOID pvPacket);
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
InitkKrnlHlp PROC NEAR
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
    jmp far ptr CODE16:Thunk16_kKrnlHlp
Thunk32_kKrnlHlp::
    movzx   eax, ax                     ; Cast USHORT to ULONG.

    pop     es
    pop     ds
    pop     ebx
    ret
InitkKrnlHlp ENDP
CODE32 ENDS

CODE16 SEGMENT
    ; Call strategy entry point for kKrnlHlp.
Thunk16_kKrnlHlp::
    call    far ptr CODE16:_strategyAsm0
    jmp far ptr FLAT:Thunk32_kKrnlHlp
CODE16 ENDS



CODE32 SEGMENT
;;
; Initiates the kKrnlLib device driver.
; @cproto    ULONG _Optlink InitkKrnlLib(PVOID pvPacket);
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
InitkKrnlLib PROC NEAR
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
    jmp far ptr CODE16:Thunk16_InitkKrnlLib
Thunk32_InitkKrnlLib::
    movzx   eax, ax                     ; Cast USHORT to ULONG.

    pop     es
    pop     ds
    pop     ebx
    ret
InitkKrnlLib ENDP
CODE32 ENDS

CODE16 SEGMENT
    ; Call strategy entry point for kKrnlLib.
Thunk16_InitkKrnlLib::
    call    far ptr CODE16:_strategyAsm1
    jmp far ptr FLAT:Thunk32_InitkKrnlLib

CODE16 ENDS


END
