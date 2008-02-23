        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetCharAngle:PROC
        PUBLIC  _GpiSetCharAngle
_GpiSetCharAngle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetCharAngle
    add    esp, 8
    pop    fs
    ret
_GpiSetCharAngle ENDP

CODE32          ENDS

                END
