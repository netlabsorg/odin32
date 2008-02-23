        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_StretchDIBits:PROC
        PUBLIC  O32_StretchDIBits
O32_StretchDIBits PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    push   dword ptr [esp+56]
    call   _O32_StretchDIBits
    add    esp, 52
    pop    fs
    ret
O32_StretchDIBits ENDP

CODE32          ENDS

                END
