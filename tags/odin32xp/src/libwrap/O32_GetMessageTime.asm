        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetMessageTime:PROC
        PUBLIC  O32_GetMessageTime
O32_GetMessageTime PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_GetMessageTime
    pop    fs
    ret
O32_GetMessageTime ENDP

CODE32          ENDS

                END
