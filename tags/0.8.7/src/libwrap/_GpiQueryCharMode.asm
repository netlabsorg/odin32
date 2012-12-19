        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharMode:PROC
        PUBLIC  _GpiQueryCharMode
_GpiQueryCharMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryCharMode
    add    esp, 4
    pop    fs
    ret
_GpiQueryCharMode ENDP

CODE32          ENDS

                END
