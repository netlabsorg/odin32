        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryModelTransformMatrix:PROC
        PUBLIC  _GpiQueryModelTransformMatrix
_GpiQueryModelTransformMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiQueryModelTransformMatrix
    add    esp, 12
    pop    fs
    ret
_GpiQueryModelTransformMatrix ENDP

CODE32          ENDS

                END
