        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryMarker:PROC
        PUBLIC  _GpiQueryMarker
_GpiQueryMarker PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryMarker
    add    esp, 4
    pop    fs
    ret
_GpiQueryMarker ENDP

CODE32          ENDS

                END
