; $Id: libGetCS.asm,v 1.1.6.1 2002-04-01 13:04:55 bird Exp $
;
; Gets the CS.
;
; Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p

;
; Exported symbols
;
    public libHelperGetCS


CODE32 segment
    assume CS:CODE32, DS:FLAT, SS:NOTHING
;;
; Gets the value of the CS register.
; @cproto    USHORT APIENTRY  libHelperGetCS(void)
; @returns   CS value.
; @author    knut st. osmundsen (bird@anduin.net)
libHelperGetCS proc near
    xor eax,eax
    mov ax,cs
    ret
libHelperGetCS endp

CODE32 ends
end
