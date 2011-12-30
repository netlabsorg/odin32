        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetAsyncKeyState:PROC
        PUBLIC  O32_GetAsyncKeyState
O32_GetAsyncKeyState PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetAsyncKeyState
    add    esp, 4
    pop    fs
    ret
O32_GetAsyncKeyState ENDP

CODE32          ENDS

                END
