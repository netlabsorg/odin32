        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetCharDirection:PROC
        PUBLIC  _GpiSetCharDirection
_GpiSetCharDirection PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetCharDirection
    add    esp, 8
    pop    fs
    ret
_GpiSetCharDirection ENDP

CODE32          ENDS

                END
