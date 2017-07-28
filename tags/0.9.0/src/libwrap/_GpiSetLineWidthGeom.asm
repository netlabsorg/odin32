        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetLineWidthGeom:PROC
        PUBLIC  _GpiSetLineWidthGeom
_GpiSetLineWidthGeom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetLineWidthGeom
    add    esp, 8
    pop    fs
    ret
_GpiSetLineWidthGeom ENDP

CODE32          ENDS

                END
