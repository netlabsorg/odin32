        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateWindowEx:PROC
        PUBLIC  O32_CreateWindowEx
O32_CreateWindowEx PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    push   dword ptr [esp+52]
    call   _O32_CreateWindowEx
    add    esp, 48
    pop    fs
    ret
O32_CreateWindowEx ENDP

CODE32          ENDS

                END
