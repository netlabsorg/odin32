        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CharUpper:PROC
        PUBLIC  O32_CharUpper
O32_CharUpper PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CharUpper
    add    esp, 4
    pop    fs
    ret
O32_CharUpper ENDP

CODE32          ENDS

                END
