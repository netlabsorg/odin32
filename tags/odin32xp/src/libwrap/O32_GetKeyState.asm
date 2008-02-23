        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetKeyState:PROC
        PUBLIC  O32_GetKeyState
O32_GetKeyState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetKeyState
    add    esp, 4
    pop    fs
    ret
O32_GetKeyState ENDP

CODE32          ENDS

                END
