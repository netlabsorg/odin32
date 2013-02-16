        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetOpenFileName:PROC
        PUBLIC  O32_GetOpenFileName
O32_GetOpenFileName PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetOpenFileName
    add    esp, 4
    pop    fs
    ret
O32_GetOpenFileName ENDP

CODE32          ENDS

                END
