        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetMessagePos:PROC
        PUBLIC  O32_GetMessagePos
O32_GetMessagePos PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetMessagePos
    pop    fs
    ret
O32_GetMessagePos ENDP

CODE32          ENDS

                END
