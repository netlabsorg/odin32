        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetDIBitsToDevice:PROC
        PUBLIC  O32_SetDIBitsToDevice
O32_SetDIBitsToDevice PROC NEAR
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
    call   _O32_SetDIBitsToDevice
    add    esp, 48
    pop    fs
    ret
O32_SetDIBitsToDevice ENDP

CODE32          ENDS

                END
