; $Id: buffer.asm,v 1.6 2000-09-02 21:08:13 bird Exp $
;
; Simple resident buffer for use when overloading tkExecPgm - obsolete!
;
; Copyright (c) 2000 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .486p


;
;   Defined Constants And Macros
;
NBR_BUFFERS     EQU 2
BUFFER_SIZE     EQU 1536

;
;   Include files
;
    include devsegdf.inc

;
;   Exported symbols
;
    public AcquireBuffer
    public ReleaseBuffer
    public QueryBufferSegmentOffset
    public QueryBufferPointerFromFilename


;
;   Imported Functions
;
    extrn stricmp:PROC


;
;   Global Variables
;
DATA16 SEGMENT
aachBuffer  db BUFFER_SIZE*NBR_BUFFERS dup(?) ; The buffer
DATA16 ENDS

DATA32 SEGMENT
afBuffer    db NBR_BUFFERS dup(0)       ; Access "semaphore"
                                        ; 0 - not in use
                                        ; 1 - in use
DATA32 ENDS


CODE32 segment
    assume CS:CODE32, DS:NOTHING, SS:NOTHING

;;
; Aquires a resident buffer. (intended use for tkExecPgm)
; @cproto    assembly only for time being.
; @returns   Pointer to buffer
; @uses      eax
; @sketch    if AfBuffer == 0 then
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
    push    ecx
    push    edx

    ; make ds flat
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    ASSUME  DS:FLAT

    ; loop thru all buffers and try reserve one
    mov     ah, 1                       ; afBuffer[ecx] is set to this value (if equal to al)
    mov     ecx, NBR_BUFFERS            ; interations.
    mov     edx, offset FLAT:afBuffer
    add     edx, ecx
AcquireBuffer_loop:
    dec     edx
    mov     al, 0                       ; afBuffer[ecx] should have this value
    lock cmpxchg [edx], ah
    je      AcquireBuffer_ok
    loop    AcquireBuffer_loop

    ; failure
AcquireBuffer_nok:
    xor     eax,eax
    jmp     AcquireBuffer_ret

    ;success - calc buffer pointer
AcquireBuffer_ok:
    mov     eax, BUFFER_SIZE
    dec     ecx
    imul    eax, ecx                    ; ecx has the buffer number
    add     eax, offset FLAT:aachBuffer
AcquireBuffer_ret:
    pop     edx
    pop     ecx
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
; @status
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark
ReleaseBuffer PROC NEAR
    ASSUME  DS:NOTHING
    push    ds
    push    ecx
    push    edx

    ; make ds flat
    push    eax
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    ASSUME  DS:FLAT
    pop     eax
    push    eax

    ; validate input and calc buffer number
    cmp     eax, offset FLAT:aachBuffer
    jl      ReleaseBuffer_nok           ; if eax (buffer pointer) is less than the first buffer, then fail.
    sub     eax, offset FLAT:aachBuffer ; eax <- offset to buffer from aachBuffer
    xor     edx, edx
    mov     ecx, BUFFER_SIZE
    div     ecx                         ; eax <- buffer number, edx <- offset into buffer (should be NULL)
    or      edx, edx
    jnz     ReleaseBuffer_nok           ; if offset into buffer not 0 the fail.
    cmp     eax, NBR_BUFFERS
    jge     ReleaseBuffer_nok           ; if buffernumber >= number of buffers then fail.

    ; unlock buffer - if locked
    mov     edx, eax
    add     edx, offset FLAT:afBuffer   ; ds:edx  points at buffer "semaphore"
    mov     al, 1
    mov     ah, 0
    lock cmpxchg [edx], ah
    jne     ReleaseBuffer_nok           ; fail if buffer was not locked

ReleaseBuffer_ok:
    ;swipe out buffer
    pop     eax
    push    edi
    push    es
    mov     edi, eax
    mov     ax, ds
    mov     es, ax
    ASSUME  es:FLAT
    xor     eax, eax                    ; ecx is allready BUFFER_SIZE
    rep     stosb
    pop     es
    pop     edi

    ;return successfully
    jmp     ReleaseBuffer_ret

ReleaseBuffer_nok:
    ;failure
    pop     eax
    mov     eax, 87 ;some error

ReleaseBuffer_ret:
    pop     edx
    pop     ecx
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

    ; make ds FLAT
    push    eax
    mov     ax, seg FLAT:DATA32
    mov     ds, ax
    ASSUME  DS:FLAT
    pop     eax

    ; validate parameter and calc offset relative to aachBuffer
    cmp     eax, offset FLAT:aachBuffer
    jl      QueryBufferSegmentOffset_nok
    sub     eax, offset FLAT:aachBuffer
    cmp     eax, NBR_BUFFERS * BUFFER_SIZE
    jge     QueryBufferSegmentOffset_nok

QueryBufferSegmentOffset_ok:
    jmp     far ptr CODE16:GetBufferSegmentOffset16
QueryBufferSegmentOffset_Back::
    pop     ds
    ret

QueryBufferSegmentOffset_nok:
    mov     eax, 00570000h
    pop     ds
    ret
QueryBufferSegmentOffset ENDP


;;
; Special function which checks all used buffers for the string passed in in eax.
; @cproto    PCHAR _Optlink QueryBufferPointerFromFilename(const char *pszFilename)
; @returns   Pointer to Buffer. NULL on error/notfound.
; @param     ds:eax   Pointer to filename. The filename will later be compared to the string at buffer start.
;                     Currently this parameter is not used, since there is only one buffer.
; @uses      eax.
; @sketch
; @status    completely implemented
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
; @remark    assumes ds flat on call.
QueryBufferPointerFromFilename PROC NEAR
    ASSUME  cs:CODE32, ds:FLAT
    push    ecx
    push    edx
    push    ebx

    ; loop thru all buffers until found or non left
    mov     ecx, NBR_BUFFERS
    mov     edx, BUFFER_SIZE
    imul    edx, ecx                    ; edx <- sizeof(aachBuffer)
    add     edx, offset FLAT:aachBuffer ; edx points at the first byte after the buffers.
    mov     ebx, offset FLAT:afBuffer
    add     ebx, ecx                    ; edx points at the first byte after the "semaphore" array (afBuffer).
QueryBufferPointerFromFilename_loop:
    sub     edx, BUFFER_SIZE            ; edx points at the buffer being concidered.
    dec     ebx                         ; ebx points at the "semaphore" for the buffer being concidered.
    cmp     byte ptr [ebx], 1           ; Is buffer in use?
    jne     QueryBufferPointerFromFilename_next

    ; buffer is active - lets compare
    push    eax
    push    ecx
    push    edx
    sub     esp, 8
    call    stricmp
    add     esp, 8
    pop     edx
    pop     ecx
    or      eax, eax
    jz      QueryBufferPointerFromFilename_found
    pop     eax

QueryBufferPointerFromFilename_next:
    loop QueryBufferPointerFromFilename_loop
    ; when we exit this loop we have failed!

QueryBufferPointerFromFilename_nok:
    ; The buffer was not found, return NULL pointer.
    xor     eax, eax
    pop     ebx
    pop     edx
    pop     ecx
    ret

QueryBufferPointerFromFilename_found:
    ; The buffer was found, return the pointer to it!
    pop     eax
    pop     ebx
    mov     eax, edx
    pop     edx
    pop     ecx
    ret

QueryBufferPointerFromFilename ENDP




CODE32 ENDS


CODE16 SEGMENT
;;
; Gets the segment(->es) and offset(+ax -> ax) of the achBuffer.
; @param    ax  offset to buffer relative to aachBuffer
; Jumps back to GetBufferOffset32
GetBufferSegmentOffset16:
    ASSUME CS:CODE16, DS:NOTHING
    push    ax
    mov     ax, seg aachBuffer
    mov     es, ax
    pop     ax
    add     ax, offset aachBuffer
    jmp     far ptr FLAT:QueryBufferSegmentOffset_Back
CODE16 ENDS

END

