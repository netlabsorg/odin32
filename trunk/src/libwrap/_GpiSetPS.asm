        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetPS:PROC
        PUBLIC  _GpiSetPS
_GpiSetPS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiSetPS
    add    esp, 12
    pop    fs
    ret
_GpiSetPS ENDP

CODE32          ENDS

                END
