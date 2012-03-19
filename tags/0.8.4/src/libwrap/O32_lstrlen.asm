        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_lstrlen:PROC
        PUBLIC  O32_lstrlen
O32_lstrlen PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_lstrlen
    add    esp, 4
    pop    fs
    ret
O32_lstrlen ENDP

CODE32          ENDS

                END
