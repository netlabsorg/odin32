        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharBreakExtra:PROC
        PUBLIC  _GpiQueryCharBreakExtra
_GpiQueryCharBreakExtra PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryCharBreakExtra
    add    esp, 8
    pop    fs
    ret
_GpiQueryCharBreakExtra ENDP

CODE32          ENDS

                END
