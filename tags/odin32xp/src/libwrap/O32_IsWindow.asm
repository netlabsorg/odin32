        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsWindow:PROC
        PUBLIC  O32_IsWindow
O32_IsWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsWindow
    add    esp, 4
    pop    fs
    ret
O32_IsWindow ENDP

CODE32          ENDS

                END
