        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ReplyMessage:PROC
        PUBLIC  O32_ReplyMessage
O32_ReplyMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ReplyMessage
    add    esp, 4
    pop    fs
    ret
O32_ReplyMessage ENDP

CODE32          ENDS

                END
