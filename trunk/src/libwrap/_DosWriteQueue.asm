        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosWriteQueue:PROC
        PUBLIC  _DosWriteQueue
_DosWriteQueue PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   DosWriteQueue
    add    esp, 20
    pop    fs
    ret
_DosWriteQueue ENDP

CODE32          ENDS

                END
