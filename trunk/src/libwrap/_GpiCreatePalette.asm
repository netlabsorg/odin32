        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreatePalette:PROC
        PUBLIC  _GpiCreatePalette
_GpiCreatePalette PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiCreatePalette
    add    esp, 20
    pop    fs
    ret
_GpiCreatePalette ENDP

CODE32          ENDS

                END
