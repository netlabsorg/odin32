        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplSetDriver:PROC
        PUBLIC  _SplSetDriver
_SplSetDriver PROC NEAR
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
    call   SplSetDriver
    add    esp, 28
    pop    fs
    ret
_SplSetDriver ENDP

CODE32          ENDS

                END
