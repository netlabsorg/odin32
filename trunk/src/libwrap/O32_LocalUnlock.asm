        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_LocalUnlock:PROC
        PUBLIC  O32_LocalUnlock
O32_LocalUnlock PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_LocalUnlock
    add    esp, 4
    pop    fs
    ret
O32_LocalUnlock ENDP

CODE32          ENDS

                END
