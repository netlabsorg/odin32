        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DdeUnaccessData:PROC
        PUBLIC  O32_DdeUnaccessData
O32_DdeUnaccessData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DdeUnaccessData
    add    esp, 4
    pop    fs
    ret
O32_DdeUnaccessData ENDP

CODE32          ENDS

                END
