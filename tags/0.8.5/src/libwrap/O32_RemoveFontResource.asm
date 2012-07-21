        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RemoveFontResource:PROC
        PUBLIC  O32_RemoveFontResource
O32_RemoveFontResource PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_RemoveFontResource
    add    esp, 4
    pop    fs
    ret
O32_RemoveFontResource ENDP

CODE32          ENDS

                END
