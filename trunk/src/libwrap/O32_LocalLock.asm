        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_LocalLock:PROC
        PUBLIC  O32_LocalLock
O32_LocalLock PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_LocalLock
    add    esp, 4
    pop    fs
    ret
O32_LocalLock ENDP

CODE32          ENDS

                END
