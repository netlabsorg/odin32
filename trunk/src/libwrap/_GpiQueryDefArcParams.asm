        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryDefArcParams:PROC
        PUBLIC  _GpiQueryDefArcParams
_GpiQueryDefArcParams PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryDefArcParams
    add    esp, 8
    pop    fs
    ret
_GpiQueryDefArcParams ENDP

CODE32          ENDS

                END
