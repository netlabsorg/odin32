        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsCharLower:PROC
        PUBLIC  O32_IsCharLower
O32_IsCharLower PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsCharLower
    add    esp, 4
    pop    fs
    ret
O32_IsCharLower ENDP

CODE32          ENDS

                END
