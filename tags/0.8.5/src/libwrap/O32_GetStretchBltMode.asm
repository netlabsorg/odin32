        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetStretchBltMode:PROC
        PUBLIC  O32_GetStretchBltMode
O32_GetStretchBltMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetStretchBltMode
    add    esp, 4
    pop    fs
    ret
O32_GetStretchBltMode ENDP

CODE32          ENDS

                END
