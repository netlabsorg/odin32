        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQueryDriver:PROC
        PUBLIC  _SplQueryDriver
_SplQueryDriver PROC NEAR
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
    call   SplQueryDriver
    add    esp, 28
    pop    fs
    ret
_SplQueryDriver ENDP

CODE32          ENDS

                END
