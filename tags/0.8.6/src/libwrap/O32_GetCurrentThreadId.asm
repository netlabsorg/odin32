        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCurrentThreadId:PROC
        PUBLIC  O32_GetCurrentThreadId
O32_GetCurrentThreadId PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCurrentThreadId
    pop    fs
    ret
O32_GetCurrentThreadId ENDP

CODE32          ENDS

                END
