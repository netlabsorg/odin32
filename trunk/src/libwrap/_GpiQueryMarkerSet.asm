        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryMarkerSet:PROC
        PUBLIC  _GpiQueryMarkerSet
_GpiQueryMarkerSet PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryMarkerSet
    add    esp, 4
    pop    fs
    ret
_GpiQueryMarkerSet ENDP

CODE32          ENDS

                END
