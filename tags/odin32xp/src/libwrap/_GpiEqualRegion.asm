        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiEqualRegion:PROC
        PUBLIC  _GpiEqualRegion
_GpiEqualRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiEqualRegion
    add    esp, 12
    pop    fs
    ret
_GpiEqualRegion ENDP

CODE32          ENDS

                END
