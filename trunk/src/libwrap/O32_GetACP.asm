        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetACP:PROC
        PUBLIC  O32_GetACP
O32_GetACP PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetACP
    pop    fs
    ret
O32_GetACP ENDP

CODE32          ENDS

                END
