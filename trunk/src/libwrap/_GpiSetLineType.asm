        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetLineType:PROC
        PUBLIC  _GpiSetLineType
_GpiSetLineType PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetLineType
    add    esp, 8
    pop    fs
    ret
_GpiSetLineType ENDP

CODE32          ENDS

                END
