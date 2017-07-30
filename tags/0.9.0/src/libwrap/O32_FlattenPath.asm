        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_FlattenPath:PROC
        PUBLIC  O32_FlattenPath
O32_FlattenPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_FlattenPath
    add    esp, 4
    pop    fs
    ret
O32_FlattenPath ENDP

CODE32          ENDS

                END
