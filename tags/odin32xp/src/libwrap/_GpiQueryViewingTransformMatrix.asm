        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryViewingTransformMatrix:PROC
        PUBLIC  _GpiQueryViewingTransformMatrix
_GpiQueryViewingTransformMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiQueryViewingTransformMatrix
    add    esp, 12
    pop    fs
    ret
_GpiQueryViewingTransformMatrix ENDP

CODE32          ENDS

                END
