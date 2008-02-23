        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiDrawBits:PROC
        PUBLIC  _GpiDrawBits
_GpiDrawBits PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    push   dword ptr [esp+32]
    call   GpiDrawBits
    add    esp, 28
    pop    fs
    ret
_GpiDrawBits ENDP

CODE32          ENDS

                END
