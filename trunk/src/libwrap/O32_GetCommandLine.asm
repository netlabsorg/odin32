        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCommandLine:PROC
        PUBLIC  O32_GetCommandLine
O32_GetCommandLine PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCommandLine
    pop    fs
    ret
O32_GetCommandLine ENDP

CODE32          ENDS

                END
