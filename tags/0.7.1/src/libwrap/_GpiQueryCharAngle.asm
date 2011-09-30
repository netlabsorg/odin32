        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharAngle:PROC
        PUBLIC  _GpiQueryCharAngle
_GpiQueryCharAngle PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryCharAngle
    add    esp, 8
    pop    fs
    ret
_GpiQueryCharAngle ENDP

CODE32          ENDS

                END
