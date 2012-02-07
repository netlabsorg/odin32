        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryCharStringPosAt:PROC
        PUBLIC  _GpiQueryCharStringPosAt
_GpiQueryCharStringPosAt PROC NEAR
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
    call   GpiQueryCharStringPosAt
    add    esp, 28
    pop    fs
    ret
_GpiQueryCharStringPosAt ENDP

CODE32          ENDS

                END
