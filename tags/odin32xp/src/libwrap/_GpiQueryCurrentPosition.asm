        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCurrentPosition:PROC
        PUBLIC  _GpiQueryCurrentPosition
_GpiQueryCurrentPosition PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryCurrentPosition
    add    esp, 8
    pop    fs
    ret
_GpiQueryCurrentPosition ENDP

CODE32          ENDS

                END
