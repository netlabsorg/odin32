        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetMapMode:PROC
        PUBLIC  O32_GetMapMode
O32_GetMapMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetMapMode
    add    esp, 4
    pop    fs
    ret
O32_GetMapMode ENDP

CODE32          ENDS

                END
