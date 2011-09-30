        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_LockWindowUpdate:PROC
        PUBLIC  O32_LockWindowUpdate
O32_LockWindowUpdate PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_LockWindowUpdate
    add    esp, 4
    pop    fs
    ret
O32_LockWindowUpdate ENDP

CODE32          ENDS

                END
