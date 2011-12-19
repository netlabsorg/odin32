        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DdeDisconnectList:PROC
        PUBLIC  O32_DdeDisconnectList
O32_DdeDisconnectList PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DdeDisconnectList
    add    esp, 4
    pop    fs
    ret
O32_DdeDisconnectList ENDP

CODE32          ENDS

                END
