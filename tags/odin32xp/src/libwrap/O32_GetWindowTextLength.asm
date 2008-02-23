        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetWindowTextLength:PROC
        PUBLIC  O32_GetWindowTextLength
O32_GetWindowTextLength PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetWindowTextLength
    add    esp, 4
    pop    fs
    ret
O32_GetWindowTextLength ENDP

CODE32          ENDS

                END
