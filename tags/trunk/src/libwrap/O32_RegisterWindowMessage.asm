        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegisterWindowMessage:PROC
        PUBLIC  O32_RegisterWindowMessage
O32_RegisterWindowMessage PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_RegisterWindowMessage
    add    esp, 4
    pop    fs
    ret
O32_RegisterWindowMessage ENDP

CODE32          ENDS

                END
