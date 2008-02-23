        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetDIBits:PROC
        PUBLIC  O32_SetDIBits
O32_SetDIBits PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   _O32_SetDIBits
    add    esp, 28
    pop    fs
    ret
O32_SetDIBits ENDP

CODE32          ENDS

                END
