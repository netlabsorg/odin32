        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EnterCriticalSection:PROC
        PUBLIC  O32_EnterCriticalSection
O32_EnterCriticalSection PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_EnterCriticalSection
    add    esp, 4
    pop    fs
    ret
O32_EnterCriticalSection ENDP

CODE32          ENDS

                END
