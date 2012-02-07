        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetStopDraw:PROC
        PUBLIC  _GpiSetStopDraw
_GpiSetStopDraw PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetStopDraw
    add    esp, 8
    pop    fs
    ret
_GpiSetStopDraw ENDP

CODE32          ENDS

                END
