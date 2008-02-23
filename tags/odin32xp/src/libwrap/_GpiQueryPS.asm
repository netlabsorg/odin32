        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryPS:PROC
        PUBLIC  _GpiQueryPS
_GpiQueryPS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiQueryPS
    add    esp, 8
    pop    fs
    ret
_GpiQueryPS ENDP

CODE32          ENDS

                END
