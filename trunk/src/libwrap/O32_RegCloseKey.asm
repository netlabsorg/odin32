        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_RegCloseKey:PROC
        PUBLIC  O32_RegCloseKey
O32_RegCloseKey PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_RegCloseKey
    add    esp, 4
    pop    fs
    ret
O32_RegCloseKey ENDP

CODE32          ENDS

                END
