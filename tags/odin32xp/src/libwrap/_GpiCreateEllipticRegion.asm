        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreateEllipticRegion:PROC
        PUBLIC  _GpiCreateEllipticRegion
_GpiCreateEllipticRegion PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiCreateEllipticRegion
    add    esp, 8
    pop    fs
    ret
_GpiCreateEllipticRegion ENDP

CODE32          ENDS

                END
