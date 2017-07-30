        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCharStringPosAt:PROC
        PUBLIC  _GpiCharStringPosAt
_GpiCharStringPosAt PROC NEAR
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
    call   GpiCharStringPosAt
    add    esp, 28
    pop    fs
    ret
_GpiCharStringPosAt ENDP

CODE32          ENDS

                END
