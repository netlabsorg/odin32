        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ReleaseMutex:PROC
        PUBLIC  O32_ReleaseMutex
O32_ReleaseMutex PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ReleaseMutex
    add    esp, 4
    pop    fs
    ret
O32_ReleaseMutex ENDP

CODE32          ENDS

                END
