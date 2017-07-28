        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCorrelateSegment:PROC
        PUBLIC  _GpiCorrelateSegment
_GpiCorrelateSegment PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   GpiCorrelateSegment
    add    esp, 28
    pop    fs
    ret
_GpiCorrelateSegment ENDP

CODE32          ENDS

                END
