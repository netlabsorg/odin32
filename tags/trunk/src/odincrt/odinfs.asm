; $Id: odinfs.asm,v 1.1 1999-08-09 11:21:46 phaller Exp $

;/*
; * Project Odin Software License can be found in LICENSE.TXT
; * Win32 Exception handling + misc functions for OS/2
; *
; * Copyright 1998 Sander van Leeuwen
; *
; */

; 1999/08/09 PH see if we can do this as INLINE functions

.386p
                NAME    odinfs

CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'
                ASSUME  CS:FLAT ,DS:FLAT,SS:FLAT

        PUBLIC GetFS
GetFS   proc near
        mov     eax, fs
        ret
GetFS   endp


        PUBLIC SetFS
SetFS   proc near
        mov     eax, [esp+4]
        mov     fs, eax
        ret
SetFS   endp


        PUBLIC SetReturnFS
SetReturnFS proc near
        push    fs
        mov     eax, [esp+8]
        mov     fs, eax
        pop     eax
        ret
SetReturnFS endp


        PUBLIC RestoreOS2FS
RestoreOS2FS proc near
        push    150bh           ; @@@PH that's NOT the clean way ! :)
        mov     ax, fs
        pop     fs
        ret
RestoreOS2FS endp

CODE32          ENDS
                END
