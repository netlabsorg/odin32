        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharExtra:PROC
        PUBLIC  _GpiQueryCharExtra
_GpiQueryCharExtra PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryCharExtra
    add    esp, 8
    pop    fs
    ret
_GpiQueryCharExtra ENDP

CODE32          ENDS

                END
