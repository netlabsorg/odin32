        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateThread:PROC
        PUBLIC  O32_CreateThread
O32_CreateThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   _O32_CreateThread
    add    esp, 24
    pop    fs
    ret
O32_CreateThread ENDP

CODE32          ENDS

                END
