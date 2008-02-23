        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetCurrentThread:PROC
        PUBLIC  O32_GetCurrentThread
O32_GetCurrentThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetCurrentThread
    pop    fs
    ret
O32_GetCurrentThread ENDP

CODE32          ENDS

                END
