        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetWindowExtEx:PROC
        PUBLIC  O32_SetWindowExtEx
O32_SetWindowExtEx PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   _O32_SetWindowExtEx
    add    esp, 16
    pop    fs
    ret
O32_SetWindowExtEx ENDP

CODE32          ENDS

                END
