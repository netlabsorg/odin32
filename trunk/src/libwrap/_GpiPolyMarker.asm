        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPolyMarker:PROC
        PUBLIC  _GpiPolyMarker
_GpiPolyMarker PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiPolyMarker
    add    esp, 12
    pop    fs
    ret
_GpiPolyMarker ENDP

CODE32          ENDS

                END
