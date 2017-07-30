        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_ShowCaret:PROC
        PUBLIC  O32_ShowCaret
O32_ShowCaret PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_ShowCaret
    add    esp, 4
    pop    fs
    ret
O32_ShowCaret ENDP

CODE32          ENDS

                END
