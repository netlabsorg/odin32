        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_InSendMessage:PROC
        PUBLIC  O32_InSendMessage
O32_InSendMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   _O32_InSendMessage
    pop    fs
    ret
O32_InSendMessage ENDP

CODE32          ENDS

                END
