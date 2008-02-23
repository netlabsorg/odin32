        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPaintRegion:PROC
        PUBLIC  _GpiPaintRegion
_GpiPaintRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiPaintRegion
    add    esp, 8
    pop    fs
    ret
_GpiPaintRegion ENDP

CODE32          ENDS

                END
