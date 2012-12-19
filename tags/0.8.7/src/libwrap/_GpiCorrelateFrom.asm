        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCorrelateFrom:PROC
        PUBLIC  _GpiCorrelateFrom
_GpiCorrelateFrom PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   GpiCorrelateFrom
    add    esp, 32
    pop    fs
    ret
_GpiCorrelateFrom ENDP

CODE32          ENDS

                END
