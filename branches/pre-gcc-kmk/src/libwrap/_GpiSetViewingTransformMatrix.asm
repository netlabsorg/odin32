        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetViewingTransformMatrix:PROC
        PUBLIC  _GpiSetViewingTransformMatrix
_GpiSetViewingTransformMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiSetViewingTransformMatrix
    add    esp, 16
    pop    fs
    ret
_GpiSetViewingTransformMatrix ENDP

CODE32          ENDS

                END
