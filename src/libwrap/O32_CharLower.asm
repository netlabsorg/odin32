        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CharLower:PROC
        PUBLIC  O32_CharLower
O32_CharLower PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CharLower
    add    esp, 4
    pop    fs
    ret
O32_CharLower ENDP

CODE32          ENDS

                END
