        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DeleteCriticalSection:PROC
        PUBLIC  O32_DeleteCriticalSection
O32_DeleteCriticalSection PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_DeleteCriticalSection
    add    esp, 4
    pop    fs
    ret
O32_DeleteCriticalSection ENDP

CODE32          ENDS

                END
