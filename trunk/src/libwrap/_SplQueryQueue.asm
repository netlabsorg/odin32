        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQueryQueue:PROC
        PUBLIC  _SplQueryQueue
_SplQueryQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    push   dword ptr [esp+28]
    call   SplQueryQueue
    add    esp, 24
    pop    fs
    ret
_SplQueryQueue ENDP

CODE32          ENDS

                END
