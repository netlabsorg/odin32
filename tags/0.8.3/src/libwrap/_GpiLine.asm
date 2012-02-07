        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiLine:PROC
        PUBLIC  _GpiLine
_GpiLine PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiLine
    add    esp, 8
    pop    fs
    ret
_GpiLine ENDP

CODE32          ENDS

                END
