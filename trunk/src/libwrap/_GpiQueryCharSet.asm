        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharSet:PROC
        PUBLIC  _GpiQueryCharSet
_GpiQueryCharSet PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryCharSet
    add    esp, 4
    pop    fs
    ret
_GpiQueryCharSet ENDP

CODE32          ENDS

                END