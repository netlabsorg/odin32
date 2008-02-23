        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplEnumQueue:PROC
        PUBLIC  _SplEnumQueue
_SplEnumQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    push   dword ptr [esp+36]
    call   SplEnumQueue
    add    esp, 32
    pop    fs
    ret
_SplEnumQueue ENDP

CODE32          ENDS

                END
