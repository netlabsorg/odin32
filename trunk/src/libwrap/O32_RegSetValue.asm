        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegSetValue:PROC
        PUBLIC  O32_RegSetValue
O32_RegSetValue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   _O32_RegSetValue
    add    esp, 20
    pop    fs
    ret
O32_RegSetValue ENDP

CODE32          ENDS

                END
