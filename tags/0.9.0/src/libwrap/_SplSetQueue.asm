        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplSetQueue:PROC
        PUBLIC  _SplSetQueue
_SplSetQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   SplSetQueue
    add    esp, 24
    pop    fs
    ret
_SplSetQueue ENDP

CODE32          ENDS

                END
