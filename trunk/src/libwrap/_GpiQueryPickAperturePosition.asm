        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPickAperturePosition:PROC
        PUBLIC  _GpiQueryPickAperturePosition
_GpiQueryPickAperturePosition PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryPickAperturePosition
    add    esp, 8
    pop    fs
    ret
_GpiQueryPickAperturePosition ENDP

CODE32          ENDS

                END
