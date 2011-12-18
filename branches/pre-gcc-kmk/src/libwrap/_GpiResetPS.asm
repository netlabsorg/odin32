        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiResetPS:PROC
        PUBLIC  _GpiResetPS
_GpiResetPS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiResetPS
    add    esp, 8
    pop    fs
    ret
_GpiResetPS ENDP

CODE32          ENDS

                END
