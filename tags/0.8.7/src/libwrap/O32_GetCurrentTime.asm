        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCurrentTime:PROC
        PUBLIC  O32_GetCurrentTime
O32_GetCurrentTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCurrentTime
    pop    fs
    ret
O32_GetCurrentTime ENDP

CODE32          ENDS

                END
