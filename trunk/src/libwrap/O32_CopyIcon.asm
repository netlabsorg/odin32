        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CopyIcon:PROC
        PUBLIC  O32_CopyIcon
O32_CopyIcon PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CopyIcon
    add    esp, 4
    pop    fs
    ret
O32_CopyIcon ENDP

CODE32          ENDS

                END
