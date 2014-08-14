        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPatternRefPoint:PROC
        PUBLIC  _GpiQueryPatternRefPoint
_GpiQueryPatternRefPoint PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryPatternRefPoint
    add    esp, 8
    pop    fs
    ret
_GpiQueryPatternRefPoint ENDP

CODE32          ENDS

                END
