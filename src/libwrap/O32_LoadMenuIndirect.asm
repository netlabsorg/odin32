        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_LoadMenuIndirect:PROC
        PUBLIC  O32_LoadMenuIndirect
O32_LoadMenuIndirect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_LoadMenuIndirect
    add    esp, 4
    pop    fs
    ret
O32_LoadMenuIndirect ENDP

CODE32          ENDS

                END
