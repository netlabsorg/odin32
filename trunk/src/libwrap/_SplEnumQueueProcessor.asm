        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplEnumQueueProcessor:PROC
        PUBLIC  _SplEnumQueueProcessor
_SplEnumQueueProcessor PROC NEAR
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
    call   SplEnumQueueProcessor
    add    esp, 32
    pop    fs
    ret
_SplEnumQueueProcessor ENDP

CODE32          ENDS

                END