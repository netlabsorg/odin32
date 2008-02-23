        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SubtractRect:PROC
        PUBLIC  O32_SubtractRect
O32_SubtractRect PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   _O32_SubtractRect
    add    esp, 12
    pop    fs
    ret
O32_SubtractRect ENDP

CODE32          ENDS

                END
