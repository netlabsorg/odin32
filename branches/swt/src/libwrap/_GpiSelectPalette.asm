        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSelectPalette:PROC
        PUBLIC  _GpiSelectPalette
_GpiSelectPalette PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSelectPalette
    add    esp, 8
    pop    fs
    ret
_GpiSelectPalette ENDP

CODE32          ENDS

                END
