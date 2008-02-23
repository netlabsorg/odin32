        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharShear:PROC
        PUBLIC  _GpiQueryCharShear
_GpiQueryCharShear PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryCharShear
    add    esp, 8
    pop    fs
    ret
_GpiQueryCharShear ENDP

CODE32          ENDS

                END
