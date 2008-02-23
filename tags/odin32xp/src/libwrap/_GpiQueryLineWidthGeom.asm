        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryLineWidthGeom:PROC
        PUBLIC  _GpiQueryLineWidthGeom
_GpiQueryLineWidthGeom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryLineWidthGeom
    add    esp, 4
    pop    fs
    ret
_GpiQueryLineWidthGeom ENDP

CODE32          ENDS

                END
