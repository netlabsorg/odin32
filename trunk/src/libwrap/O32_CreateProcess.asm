        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateProcess:PROC
        PUBLIC  O32_CreateProcess
O32_CreateProcess PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    push   dword ptr [esp+44]
    call   _O32_CreateProcess
    add    esp, 40
    pop    fs
    ret
O32_CreateProcess ENDP

CODE32          ENDS

                END
