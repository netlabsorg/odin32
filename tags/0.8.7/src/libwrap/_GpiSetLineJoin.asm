        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetLineJoin:PROC
        PUBLIC  _GpiSetLineJoin
_GpiSetLineJoin PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetLineJoin
    add    esp, 8
    pop    fs
    ret
_GpiSetLineJoin ENDP

CODE32          ENDS

                END
