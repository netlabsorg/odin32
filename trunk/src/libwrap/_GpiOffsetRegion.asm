        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiOffsetRegion:PROC
        PUBLIC  _GpiOffsetRegion
_GpiOffsetRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiOffsetRegion
    add    esp, 12
    pop    fs
    ret
_GpiOffsetRegion ENDP

CODE32          ENDS

                END
