; $Id: myldrGetFileName.asm,v 1.2 2001-02-23 02:57:54 bird Exp $
;
; myldrGetFileName2 - assembly helper for ldrGetFileName.
;           This makes ldrGetFileName work for filenames which
;           doesn't have a path, and return correct *ppachExt.
;
; Copyright (c) 2000 knut st. osmundsen
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Include files
;
    include devsegdf.inc
    include options.inc

;
; Exported symbols
;
    public _ldrGetFileName2@12

;
; Externs
;
    extrn _ldrGetFileName@12:PROC        ; calltab entry


CODE32 segment
CODE32START label byte

;;
; Assembly helper for ldrGetFileName.
; This makes ldrGetFileName work for filenames which doesn't have a path.
; @cproto   ULONG LDRCALL    ldrGetFileName2(PSZ pszFilename, PCHAR *ppachName, PCHAR *ppachExt);
; @status   completely implemented.
; @author   knut st. osmundsen
; @remark   This function relies heavily on the implementation of ldrGetFileName.
;           We help ldrGetFileName by initiating the register which hold the start of
;           the filename, edx, to the the correct value for filenames without path or
;           with only a driveletter specified.
_ldrGetFileName2@12 PROC NEAR
    ASSUME ds:FLAT

    ;
    ; Initiate the edx register incase there is no path or just a driveletter.
    ;
    mov     edx,  dword ptr [esp + 4]
    cmp     byte ptr [edx], 0
    je      lgfn_call
    cmp     byte ptr [edx+1], ':'
    jne     lgfn_call
    inc     edx

lgfn_call:
    ;
    ; Re-push the arguments and call the real function.
    ;
    push    dword ptr [esp + 0ch]
    push    dword ptr [esp + 0ch]
    push    dword ptr [esp + 0ch]
    call    _ldrGetFileName@12

    ;
    ; Check if the extention pointer is pointer after the terminator ('\0') char.
    ;
    mov     ecx, dword ptr [esp + 08h]  ; fetch the ppachName parameter.
    mov     ecx, [ecx]                  ; fetch the name pointer returned by ldrGetFileName.
    cmp     byte ptr [ecx], 0           ; Check if no name (nor extention).
    je      lgfn_zeroppachExt            ; Jump if no name.

    mov     edx, dword ptr [esp + 0ch]  ; fetch the ppachExt parameter.
    mov     edx, [edx]                  ; fetch the extention pointer returned by ldrGetFileName.
    dec     edx                         ; decrement the pointer.

    cmp     edx, ecx                    ; check if the decremented pointer is legal.
    jl      lgfn_ret

    cmp     byte ptr [edx], 0           ; If the pointer before *ppachExt is '\0' then *ppachExt should be NULL.
    jne     lgfn_ret                    ; Jump if *ppachExt is ok.

lgfn_zeroppachExt:
    mov     edx, dword ptr [esp + 0ch]  ; fetch the ppachExt parameter.
    mov     dword ptr [edx], 0          ; = NULL.

lgfn_ret:
    ret     0ch
_ldrGetFileName2@12 ENDP

CODE32 ends

END



