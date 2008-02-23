        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDeletePalette:PROC
        PUBLIC  _GpiDeletePalette
_GpiDeletePalette PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiDeletePalette
    add    esp, 4
    pop    fs
    ret
_GpiDeletePalette ENDP

CODE32          ENDS

                END
