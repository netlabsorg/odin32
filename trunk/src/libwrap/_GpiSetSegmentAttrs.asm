        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetSegmentAttrs:PROC
        PUBLIC  _GpiSetSegmentAttrs
_GpiSetSegmentAttrs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiSetSegmentAttrs
    add    esp, 16
    pop    fs
    ret
_GpiSetSegmentAttrs ENDP

CODE32          ENDS

                END
