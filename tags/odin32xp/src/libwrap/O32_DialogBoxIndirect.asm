        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_DialogBoxIndirect:PROC
        PUBLIC  O32_DialogBoxIndirect
O32_DialogBoxIndirect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   _O32_DialogBoxIndirect
    add    esp, 16
    pop    fs
    ret
O32_DialogBoxIndirect ENDP

CODE32          ENDS

                END
