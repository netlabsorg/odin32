        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetBkColor:PROC
        PUBLIC  O32_GetBkColor
O32_GetBkColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetBkColor
    add    esp, 4
    pop    fs
    ret
O32_GetBkColor ENDP

CODE32          ENDS

                END
