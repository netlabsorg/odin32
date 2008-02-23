        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetDrawingMode:PROC
        PUBLIC  _GpiSetDrawingMode
_GpiSetDrawingMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetDrawingMode
    add    esp, 8
    pop    fs
    ret
_GpiSetDrawingMode ENDP

CODE32          ENDS

                END
