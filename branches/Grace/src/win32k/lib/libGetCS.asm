; $Id: libGetCS.asm,v 1.1 2000-02-26 20:19:26 bird Exp $
;
; Gets the CS.
;
; Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
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
; @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
libHelperGetCS proc near
    xor eax,eax
    mov ax,cs
    ret
libHelperGetCS endp

CODE32 ends
end
