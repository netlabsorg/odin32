        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CharNext:PROC
        PUBLIC  O32_CharNext
O32_CharNext PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CharNext
    add    esp, 4
    pop    fs
    ret
O32_CharNext ENDP

CODE32          ENDS

                END
