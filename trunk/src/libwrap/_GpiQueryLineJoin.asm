        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryLineJoin:PROC
        PUBLIC  _GpiQueryLineJoin
_GpiQueryLineJoin PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryLineJoin
    add    esp, 4
    pop    fs
    ret
_GpiQueryLineJoin ENDP

CODE32          ENDS

                END
