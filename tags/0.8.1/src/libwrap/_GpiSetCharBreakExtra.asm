        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetCharBreakExtra:PROC
        PUBLIC  _GpiSetCharBreakExtra
_GpiSetCharBreakExtra PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetCharBreakExtra
    add    esp, 8
    pop    fs
    ret
_GpiSetCharBreakExtra ENDP

CODE32          ENDS

                END
