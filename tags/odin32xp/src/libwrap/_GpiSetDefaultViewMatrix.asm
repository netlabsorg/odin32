        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetDefaultViewMatrix:PROC
        PUBLIC  _GpiSetDefaultViewMatrix
_GpiSetDefaultViewMatrix PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiSetDefaultViewMatrix
    add    esp, 16
    pop    fs
    ret
_GpiSetDefaultViewMatrix ENDP

CODE32          ENDS

                END
