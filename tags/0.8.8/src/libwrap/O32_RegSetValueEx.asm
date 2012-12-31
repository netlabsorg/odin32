        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegSetValueEx:PROC
        PUBLIC  O32_RegSetValueEx
O32_RegSetValueEx PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   _O32_RegSetValueEx
    add    esp, 24
    pop    fs
    ret
O32_RegSetValueEx ENDP

CODE32          ENDS

                END
