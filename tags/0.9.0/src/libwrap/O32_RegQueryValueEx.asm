        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegQueryValueEx:PROC
        PUBLIC  O32_RegQueryValueEx
O32_RegQueryValueEx PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   _O32_RegQueryValueEx
    add    esp, 24
    pop    fs
    ret
O32_RegQueryValueEx ENDP

CODE32          ENDS

                END
