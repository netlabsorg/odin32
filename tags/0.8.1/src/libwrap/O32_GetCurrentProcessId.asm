        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCurrentProcessId:PROC
        PUBLIC  O32_GetCurrentProcessId
O32_GetCurrentProcessId PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCurrentProcessId
    pop    fs
    ret
O32_GetCurrentProcessId ENDP

CODE32          ENDS

                END
