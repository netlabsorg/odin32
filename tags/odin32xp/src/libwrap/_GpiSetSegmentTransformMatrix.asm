        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetSegmentTransformMatrix:PROC
        PUBLIC  _GpiSetSegmentTransformMatrix
_GpiSetSegmentTransformMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiSetSegmentTransformMatrix
    add    esp, 20
    pop    fs
    ret
_GpiSetSegmentTransformMatrix ENDP

CODE32          ENDS

                END
