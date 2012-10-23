        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiPointArc:PROC
        PUBLIC  _GpiPointArc
_GpiPointArc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiPointArc
    add    esp, 8
    pop    fs
    ret
_GpiPointArc ENDP

CODE32          ENDS

                END
