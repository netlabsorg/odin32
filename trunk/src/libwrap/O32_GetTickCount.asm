        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetTickCount:PROC
        PUBLIC  O32_GetTickCount
O32_GetTickCount PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetTickCount
    pop    fs
    ret
O32_GetTickCount ENDP

CODE32          ENDS

                END
