        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiBox:PROC
        PUBLIC  _GpiBox
_GpiBox PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiBox
    add    esp, 20
    pop    fs
    ret
_GpiBox ENDP

CODE32          ENDS

                END
