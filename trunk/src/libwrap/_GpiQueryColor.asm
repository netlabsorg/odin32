        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryColor:PROC
        PUBLIC  _GpiQueryColor
_GpiQueryColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryColor
    add    esp, 4
    pop    fs
    ret
_GpiQueryColor ENDP

CODE32          ENDS

                END
