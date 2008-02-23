        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetInitialSegmentAttrs:PROC
        PUBLIC  _GpiSetInitialSegmentAttrs
_GpiSetInitialSegmentAttrs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiSetInitialSegmentAttrs
    add    esp, 12
    pop    fs
    ret
_GpiSetInitialSegmentAttrs ENDP

CODE32          ENDS

                END
