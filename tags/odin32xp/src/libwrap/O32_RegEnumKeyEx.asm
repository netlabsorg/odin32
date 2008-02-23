        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegEnumKeyEx:PROC
        PUBLIC  O32_RegEnumKeyEx
O32_RegEnumKeyEx PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   _O32_RegEnumKeyEx
    add    esp, 32
    pop    fs
    ret
O32_RegEnumKeyEx ENDP

CODE32          ENDS

                END
