        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosCreateQueue:PROC
        PUBLIC  _DosCreateQueue
_DosCreateQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosCreateQueue
    add    esp, 12
    pop    fs
    ret
_DosCreateQueue ENDP

CODE32          ENDS

                END
