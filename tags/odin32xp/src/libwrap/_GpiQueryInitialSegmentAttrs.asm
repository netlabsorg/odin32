        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryInitialSegmentAttrs:PROC
        PUBLIC  _GpiQueryInitialSegmentAttrs
_GpiQueryInitialSegmentAttrs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryInitialSegmentAttrs
    add    esp, 8
    pop    fs
    ret
_GpiQueryInitialSegmentAttrs ENDP

CODE32          ENDS

                END
