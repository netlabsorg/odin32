        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetDefArcParams:PROC
        PUBLIC  _GpiSetDefArcParams
_GpiSetDefArcParams PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetDefArcParams
    add    esp, 8
    pop    fs
    ret
_GpiSetDefArcParams ENDP

CODE32          ENDS

                END
