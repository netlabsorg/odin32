        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetCurrentDirectory:PROC
        PUBLIC  O32_SetCurrentDirectory
O32_SetCurrentDirectory PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetCurrentDirectory
    add    esp, 4
    pop    fs
    ret
O32_SetCurrentDirectory ENDP

CODE32          ENDS

                END
