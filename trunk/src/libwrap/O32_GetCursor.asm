        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCursor:PROC
        PUBLIC  O32_GetCursor
O32_GetCursor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCursor
    pop    fs
    ret
O32_GetCursor ENDP

CODE32          ENDS

                END
