        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateSolidBrush:PROC
        PUBLIC  O32_CreateSolidBrush
O32_CreateSolidBrush PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CreateSolidBrush
    add    esp, 4
    pop    fs
    ret
O32_CreateSolidBrush ENDP

CODE32          ENDS

                END
