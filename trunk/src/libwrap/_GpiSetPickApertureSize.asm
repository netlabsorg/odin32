        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetPickApertureSize:PROC
        PUBLIC  _GpiSetPickApertureSize
_GpiSetPickApertureSize PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiSetPickApertureSize
    add    esp, 12
    pop    fs
    ret
_GpiSetPickApertureSize ENDP

CODE32          ENDS

                END
