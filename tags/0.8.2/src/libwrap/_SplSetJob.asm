        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplSetJob:PROC
        PUBLIC  _SplSetJob
_SplSetJob PROC NEAR
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
    call   SplSetJob
    add    esp, 28
    pop    fs
    ret
_SplSetJob ENDP

CODE32          ENDS

                END
