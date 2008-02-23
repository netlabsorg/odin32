        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DdeFreeDataHandle:PROC
        PUBLIC  O32_DdeFreeDataHandle
O32_DdeFreeDataHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DdeFreeDataHandle
    add    esp, 4
    pop    fs
    ret
O32_DdeFreeDataHandle ENDP

CODE32          ENDS

                END
