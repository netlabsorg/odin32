        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_HideCaret:PROC
        PUBLIC  O32_HideCaret
O32_HideCaret PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_HideCaret
    add    esp, 4
    pop    fs
    ret
O32_HideCaret ENDP

CODE32          ENDS

                END
