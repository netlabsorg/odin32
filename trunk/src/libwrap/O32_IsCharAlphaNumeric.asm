        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_IsCharAlphaNumeric:PROC
        PUBLIC  O32_IsCharAlphaNumeric
O32_IsCharAlphaNumeric PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_IsCharAlphaNumeric
    add    esp, 4
    pop    fs
    ret
O32_IsCharAlphaNumeric ENDP

CODE32          ENDS

                END
