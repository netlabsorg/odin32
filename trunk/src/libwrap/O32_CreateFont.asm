        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CreateFont:PROC
        PUBLIC  O32_CreateFont
O32_CreateFont PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    push   dword ptr [esp+60]
    call   _O32_CreateFont
    add    esp, 56
    pop    fs
    ret
O32_CreateFont ENDP

CODE32          ENDS

                END
