        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPatternSet:PROC
        PUBLIC  _GpiQueryPatternSet
_GpiQueryPatternSet PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryPatternSet
    add    esp, 4
    pop    fs
    ret
_GpiQueryPatternSet ENDP

CODE32          ENDS

                END
