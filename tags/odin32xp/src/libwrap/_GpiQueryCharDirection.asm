        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharDirection:PROC
        PUBLIC  _GpiQueryCharDirection
_GpiQueryCharDirection PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryCharDirection
    add    esp, 4
    pop    fs
    ret
_GpiQueryCharDirection ENDP

CODE32          ENDS

                END
