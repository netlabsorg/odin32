        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosPeekQueue:PROC
        PUBLIC  _DosPeekQueue
_DosPeekQueue PROC NEAR
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
    call   DosPeekQueue
    add    esp, 32
    pop    fs
    ret
_DosPeekQueue ENDP

CODE32          ENDS

                END
