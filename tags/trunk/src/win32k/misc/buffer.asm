; $Id: buffer.asm,v 1.1 2000-02-18 14:55:08 bird Exp $
;
; Simple resident buffer for use when overloading tkExecPgm.
;
; Copyright (c) 2000 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .486p

;
;   Include files
;
    include devsegdf.inc

;
;   Exported symbols
;
    public AcquireBuffer
    public ReleaseBuffer
    public cbBuffer


;
;   Global Variables
;
DATA32 segment
fBuffer     db 0            ;Access "semaphore"
cbBuffer    db 4096
achBuffer   db 4096 dup(?)
DATA32 ends


CODE32 segment
    assume CS:CODE32, DS:NOTHING, SS:NOTHING

;;
; Aquires a resident buffer. (intended use for tkExecPgm)
; @cproto    assembly only for time being.
; @returns   Pointer to buffer
; @uses      eax
; @sketch    if fBuffer == 0 then
;               ok!
;               fBuffer <- 1
;               return pointer to buffer
;            else
;               fail
;               return NULL
;            endif
; @status    completely implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    cbBuffer holds the size of the buffer.
AcquireBuffer PROC NEAR
    push    ds
    mov     ax, FLAT
    mov     ds, ax
    ASSUME  DS:FLAT
    mov     al, 0
    mov     ah, 1
    lock cmpxchg fBuffer, ah
    jnz     AcquireBuffer_nok
AcquireBuffer_ok:
    mov     eax, offset achBuffer
    pop     ds
    ret
AcquireBuffer_nok:
    xor     eax,eax
    pop     ds
    ret
AcquireBuffer ENDP

;;
; Release the resident buffer pointed to by eax from use.
; @cproto    assembly only for time being.
; @returns   0 on success, 87 on error.
; @param     eax  Pointer to buffer.
; @uses      eax
; @equiv
; @sketch    if eax == achBuffer then
;                set fBuffer to 0 if 1.
;                if fBuffer was not 1 then fail with rc = 87!
;            else
;                fail with rc = 87
;            endif
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
ReleaseBuffer PROC NEAR
    ASSUME  DS:NOTHING
    push    ds
    mov     ax, FLAT
    mov     ds, ax
    ASSUME  DS:FLAT
    cmp     eax, offset achBuffer
    jne     ReleaseBuffer_nok
    mov     al, 1
    mov     ah, 0
    lock cmpxchg fBuffer, ah
    jnz     ReleaseBuffer_nok
ReleaseBuffer_ok:
    xor     eax, eax
    pop     ds
    ret
ReleaseBuffer_nok:
    mov     eax, 87 ;some error
    pop     ds
    ret
ReleaseBuffer ENDP


;;
; Gets the 16-bit segment and offset for this buffer.
; @cproto    assembly only for time being.
; @returns   segment in es and offset in eax
;            On error high word of eax is 87.
; @param     eax  Buffer pointer.
; @uses      eax, es
; @status    completely implemented.
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    cbBuffer holds the size of the buffer.
QueryBufferSegmentOffset PROC NEAR
    ASSUME  DS:NOTHING
    push    ds
    mov     ax, FLAT
    mov     ds, ax
    ASSUME  DS:FLAT
    cmp     eax, offset achBuffer
    jne     QueryBufferSegmentOffset_nok
    cmp     fBuffer, 1
    jne     QueryBufferSegmentOffset_nok

QueryBufferSegmentOffset_ok:
    xor     eax, eax
    jmp     far ptr CODE16:GetBufferSegmentOffset16
QueryBufferSegmentOffset_Back::
    pop     ds
    ret

QueryBufferSegmentOffset_nok:
    mov     eax, 00570000h
    pop     ds
    ret
QueryBufferSegmentOffset ENDP


CODE32 ENDS

CODE16 SEGMENT

;;
; Gets the segment(->es) and offset(->ax) of the achBuffer.
; Jumps back to GetBufferOffset32
GetBufferSegmentOffset16:
    ASSUME CS:CODE16, DS:FLAT
    mov     ax, seg achBuffer
    mov     es, ax
    mov     ax, offset achBuffer
    jmp     far ptr FLAT:QueryBufferSegmentOffset_Back
CODE16 ENDS

END

