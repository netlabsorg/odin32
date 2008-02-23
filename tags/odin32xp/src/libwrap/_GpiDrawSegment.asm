        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDrawSegment:PROC
        PUBLIC  _GpiDrawSegment
_GpiDrawSegment PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiDrawSegment
    add    esp, 8
    pop    fs
    ret
_GpiDrawSegment ENDP

CODE32          ENDS

                END
