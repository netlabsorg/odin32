; $Id: krnlLockedWrite.asm,v 1.2 2002-12-19 01:49:09 bird Exp $
;
; Locked Write functions used by krnlImport to overload
; and restore functions.
;
; Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
;
; Project Odin Software License can be found in LICENSE.TXT
;
    .386p


;*******************************************************************************
;* Header Files                                                                *
;*******************************************************************************
include devSegDf.inc


;*******************************************************************************
;* Exported Symbols                                                            *
;*******************************************************************************
public krnlLockedWrite



CODE32 segment


;;
; Writes 5 bytes of code in a secure(?) way.
; @cproto   void _Optlink LockedWrite(unsigned long ulAddr, unsigned char chOpcode, unsigned long ulDword);
; @returns  0.
; @param    pvAddr  (eax)   Where to write.
; @param    chOpcode (dl)   The opcode to write - ie. the first byte.
; @param    ulDword (ecx)   A dword (4 bytes) following the opcode.
; @uses     eax, edx, ecx
; @author   knut st. osmundsen (kosmunds@csc.com)
krnlLockedWrite proc near
    push    ebx                         ; Save ebx accoring to _Optlink.
    pushfd                              ; Save flags so we restore interrupts correctly.

    ; Disable interrupts.
    cli                                 ; Clear interrupts.

    ; Disable write R0 protection
    mov     ebx, cr0                    ; Get current cr0
    push    ebx                         ; Save it so we may restore it correctly
    and     ebx, 0fffeffffh             ; Clear the 16th (WP) bit.
    mov     cr0, ebx                    ; Restore cr0 without WP.

    ;
    ; Do write: We're to write 5 bytes.
    ;
    lock mov word ptr [eax],0FEEBh      ; Write spin jump (jmp $-1)
    nop                                 ; Just wait a second in case someone is
    nop                                 ;   executing the next few bytes.

    mov     ebx, ecx
    shr     ebx, 16                     ; bx = high word of last dword.
    lock mov word ptr [eax + 3], bx     ; Write the last two bytes.

    shl     ecx, 8
    mov     cl, dl                      ; ecx is now the dword starting at eax.
    lock mov dword ptr [eax], ecx       ; Write the first dword.

    ; restore WP and interrupts.
    pop     ebx
    mov     cr0, ebx                    ; Restore cr0 with WP as on calltime.
    popfd                               ; Restore interrupt flag.
    pop     ebx                         ; Restore ebx according to _Optlink.
    xor     eax, eax                    ; How could we fail?
    ret
krnlLockedWrite endp



CODE32 ends

end
