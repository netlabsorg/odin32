        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplCreateQueue:PROC
        PUBLIC  _SplCreateQueue
_SplCreateQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   SplCreateQueue
    add    esp, 16
    pop    fs
    ret
_SplCreateQueue ENDP

CODE32          ENDS

                END
