        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiScale:PROC
        PUBLIC  _GpiScale
_GpiScale PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiScale
    add    esp, 20
    pop    fs
    ret
_GpiScale ENDP

CODE32          ENDS

                END
