        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDrawFrom:PROC
        PUBLIC  _GpiDrawFrom
_GpiDrawFrom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiDrawFrom
    add    esp, 12
    pop    fs
    ret
_GpiDrawFrom ENDP

CODE32          ENDS

                END
