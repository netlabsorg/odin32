        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreatePaletteFromPMHandle:PROC
        PUBLIC  O32_CreatePaletteFromPMHandle
O32_CreatePaletteFromPMHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CreatePaletteFromPMHandle
    add    esp, 4
    pop    fs
    ret
O32_CreatePaletteFromPMHandle ENDP

CODE32          ENDS

                END
