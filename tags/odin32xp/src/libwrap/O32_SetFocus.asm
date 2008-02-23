        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetFocus:PROC
        PUBLIC  O32_SetFocus
O32_SetFocus PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetFocus
    add    esp, 4
    pop    fs
    ret
O32_SetFocus ENDP

CODE32          ENDS

                END
