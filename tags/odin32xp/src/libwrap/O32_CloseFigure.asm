        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_CloseFigure:PROC
        PUBLIC  O32_CloseFigure
O32_CloseFigure PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_CloseFigure
    add    esp, 4
    pop    fs
    ret
O32_CloseFigure ENDP

CODE32          ENDS

                END
