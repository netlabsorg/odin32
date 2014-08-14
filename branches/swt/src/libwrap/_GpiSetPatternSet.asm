        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetPatternSet:PROC
        PUBLIC  _GpiSetPatternSet
_GpiSetPatternSet PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetPatternSet
    add    esp, 8
    pop    fs
    ret
_GpiSetPatternSet ENDP

CODE32          ENDS

                END
