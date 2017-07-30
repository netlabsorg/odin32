        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetLastError:PROC
        PUBLIC  O32_GetLastError
O32_GetLastError PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetLastError
    pop    fs
    ret
O32_GetLastError ENDP

CODE32          ENDS

                END
