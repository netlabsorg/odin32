        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiMarker:PROC
        PUBLIC  _GpiMarker
_GpiMarker PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiMarker
    add    esp, 8
    pop    fs
    ret
_GpiMarker ENDP

CODE32          ENDS

                END
