        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetModuleHandle:PROC
        PUBLIC  O32_GetModuleHandle
O32_GetModuleHandle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_GetModuleHandle
    add    esp, 4
    pop    fs
    ret
O32_GetModuleHandle ENDP

CODE32          ENDS

                END
