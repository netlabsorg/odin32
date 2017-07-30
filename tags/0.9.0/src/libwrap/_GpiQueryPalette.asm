        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPalette:PROC
        PUBLIC  _GpiQueryPalette
_GpiQueryPalette PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryPalette
    add    esp, 4
    pop    fs
    ret
_GpiQueryPalette ENDP

CODE32          ENDS

                END
