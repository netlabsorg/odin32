        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetEnvironmentStrings:PROC
        PUBLIC  O32_GetEnvironmentStrings
O32_GetEnvironmentStrings PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetEnvironmentStrings
    pop    fs
    ret
O32_GetEnvironmentStrings ENDP

CODE32          ENDS

                END
