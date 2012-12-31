        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCloseFigure:PROC
        PUBLIC  _GpiCloseFigure
_GpiCloseFigure PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiCloseFigure
    add    esp, 4
    pop    fs
    ret
_GpiCloseFigure ENDP

CODE32          ENDS

                END
