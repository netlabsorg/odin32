        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetBackColor:PROC
        PUBLIC  _GpiSetBackColor
_GpiSetBackColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetBackColor
    add    esp, 8
    pop    fs
    ret
_GpiSetBackColor ENDP

CODE32          ENDS

                END
