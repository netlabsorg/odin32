; $Id: myldrOpenPatha.asm,v 1.1 2000-09-22 09:22:40 bird Exp $
;
; myldrOpenPatha - assembly helpers for myldrOpenPath.
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
    public _ldrOpenPath@20

;
; Externs
;
    extrn __ldrOpenPath:PROC            ; calltab entry
    extrn _options:options


CODE32 segment
CODE32START label byte


;;
; Wrapper for ldrOpenPath which in build 14053 got a new parameter.
; @status    completely implemented.
; @author    knut st. osmundsen
_ldrOpenPath@20 PROC NEAR
    ASSUME ds:FLAT
    cmp     FLAT:DATA16:_options.ulBuild, 14053
    jge      new

    ;
    ; We does now have one parameter more than necessary on the stack.
    ;
    add     esp, 4
    mov     eax, [esp + 0ch]
    mov     [esp + 010h], eax
    mov     eax, [esp + 08h]
    mov     [esp +  0ch], eax
    mov     eax, [esp + 04h]
    mov     [esp +  08h], eax
    mov     eax, [esp + 00h]
    mov     [esp +  04h], eax
    mov     eax, [esp - 04h]
    mov     [esp +  00h], eax

new:
    jmp     near ptr FLAT:CALLTAB:__ldrOpenPath

_ldrOpenPath@20 ENDP



CODE32 ends

END


