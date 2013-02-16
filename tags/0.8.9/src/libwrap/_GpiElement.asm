        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiElement:PROC
        PUBLIC  _GpiElement
_GpiElement PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiElement
    add    esp, 20
    pop    fs
    ret
_GpiElement ENDP

CODE32          ENDS

                END
