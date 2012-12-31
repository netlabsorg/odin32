        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetFocus:PROC
        PUBLIC  O32_GetFocus
O32_GetFocus PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetFocus
    pop    fs
    ret
O32_GetFocus ENDP

CODE32          ENDS

                END
