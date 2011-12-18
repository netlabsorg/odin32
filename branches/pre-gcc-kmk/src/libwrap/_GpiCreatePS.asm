        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreatePS:PROC
        PUBLIC  _GpiCreatePS
_GpiCreatePS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiCreatePS
    add    esp, 16
    pop    fs
    ret
_GpiCreatePS ENDP

CODE32          ENDS

                END
