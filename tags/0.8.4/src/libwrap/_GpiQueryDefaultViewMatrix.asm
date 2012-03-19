        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryDefaultViewMatrix:PROC
        PUBLIC  _GpiQueryDefaultViewMatrix
_GpiQueryDefaultViewMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiQueryDefaultViewMatrix
    add    esp, 12
    pop    fs
    ret
_GpiQueryDefaultViewMatrix ENDP

CODE32          ENDS

                END
