        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ClientToScreen:PROC
        PUBLIC  O32_ClientToScreen
O32_ClientToScreen PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   _O32_ClientToScreen
    add    esp, 8
    pop    fs
    ret
O32_ClientToScreen ENDP

CODE32          ENDS

                END
