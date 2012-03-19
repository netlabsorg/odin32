        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetWindowPos:PROC
        PUBLIC  O32_SetWindowPos
O32_SetWindowPos PROC NEAR
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
    call   _O32_SetWindowPos
    add    esp, 28
    pop    fs
    ret
O32_SetWindowPos ENDP

CODE32          ENDS

                END
