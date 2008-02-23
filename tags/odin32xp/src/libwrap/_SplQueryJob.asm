        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQueryJob:PROC
        PUBLIC  _SplQueryJob
_SplQueryJob PROC NEAR
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
    call   SplQueryJob
    add    esp, 28
    pop    fs
    ret
_SplQueryJob ENDP

CODE32          ENDS

                END
