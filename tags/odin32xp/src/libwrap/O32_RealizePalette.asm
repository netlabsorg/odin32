        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RealizePalette:PROC
        PUBLIC  O32_RealizePalette
O32_RealizePalette PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_RealizePalette
    add    esp, 4
    pop    fs
    ret
O32_RealizePalette ENDP

CODE32          ENDS

                END
