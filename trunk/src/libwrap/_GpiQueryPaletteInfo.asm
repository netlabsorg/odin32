        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPaletteInfo:PROC
        PUBLIC  _GpiQueryPaletteInfo
_GpiQueryPaletteInfo PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   GpiQueryPaletteInfo
    add    esp, 24
    pop    fs
    ret
_GpiQueryPaletteInfo ENDP

CODE32          ENDS

                END
