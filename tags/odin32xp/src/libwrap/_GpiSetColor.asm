        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetColor:PROC
        PUBLIC  _GpiSetColor
_GpiSetColor PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetColor
    add    esp, 8
    pop    fs
    ret
_GpiSetColor ENDP

CODE32          ENDS

                END
