        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQueryPort:PROC
        PUBLIC  _SplQueryPort
_SplQueryPort PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   SplQueryPort
    add    esp, 24
    pop    fs
    ret
_SplQueryPort ENDP

CODE32          ENDS

                END
